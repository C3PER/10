"""
阶段五：从 PyTorch 模型自动生成 float + 静态缓冲区 C 推理代码（DSP 适配）
"""
import sys
from pathlib import Path
import torch
import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
import config
from model import BCResNet

INDENT = "    "


def generate_c_code(model, output_dir):
    """生成完整的 C 推理代码 (kws_inference.c)，float + 静态缓冲区"""
    state = model.state_dict()

    weight_data = {}
    param_names = {}

    def register(name, tensor):
        weight_data[name] = tensor.detach().cpu().numpy()
        param_names[id(tensor)] = name
        return name

    # === Head ===
    register("head_conv_w", state["head_conv.weight"])
    register("head_bn_w", state["head_bn.weight"])
    register("head_bn_b", state["head_bn.bias"])
    register("head_bn_mean", state["head_bn.running_mean"])
    register("head_bn_var", state["head_bn.running_var"])

    # === Stages ===
    stage_configs = config.STAGE_CONFIGS
    all_blocks = []
    for s_idx, (cin, cout, n_blocks, dilation, first_stride) in enumerate(stage_configs):
        for b in range(n_blocks):
            b_cin = cin if b == 0 else cout
            b_stride = first_stride if b == 0 else 1
            use_skip = (b > 0)
            prefix = f"stage{s_idx}_block{b}"
            block = model.stages[s_idx].blocks[b]

            block_info = {
                "prefix": prefix,
                "in_c": b_cin, "out_c": cout,
                "freq_stride": b_stride,
                "dilation": dilation,
                "expansion": block.expansion,
                "use_skip": use_skip,
                "weights": {}
            }

            def regb(name, tensor):
                full = f"{prefix}_{name}"
                register(full, tensor)
                block_info["weights"][name] = full

            regb("main_conv1x1_w", block.main_conv1x1.weight)
            regb("main_bn1_w", block.main_bn1.weight)
            regb("main_bn1_b", block.main_bn1.bias)
            regb("main_bn1_mean", block.main_bn1.running_mean)
            regb("main_bn1_var", block.main_bn1.running_var)
            regb("main_dwconv_w", block.main_dwconv.weight)
            regb("main_bn2_w", block.main_bn2.weight)
            regb("main_bn2_b", block.main_bn2.bias)
            regb("main_bn2_mean", block.main_bn2.running_mean)
            regb("main_bn2_var", block.main_bn2.running_var)
            regb("expand_bn_w", block.expand_bn.weight)
            regb("expand_bn_b", block.expand_bn.bias)
            regb("expand_bn_mean", block.expand_bn.running_mean)
            regb("expand_bn_var", block.expand_bn.running_var)
            regb("br_dwconv_w", block.broadcast_dwconv.weight)
            regb("br_bn_w", block.broadcast_bn.weight)
            regb("br_bn_b", block.broadcast_bn.bias)
            regb("br_bn_mean", block.broadcast_bn.running_mean)
            regb("br_bn_var", block.broadcast_bn.running_var)
            regb("br_proj_w", block.broadcast_proj.weight)

            all_blocks.append(block_info)

    # === Final layers ===
    register("final_dwconv_w", state["final_dwconv.weight"])
    register("final_dwconv_bn_w", state["final_dwconv_bn.weight"])
    register("final_dwconv_bn_b", state["final_dwconv_bn.bias"])
    register("final_dwconv_bn_mean", state["final_dwconv_bn.running_mean"])
    register("final_dwconv_bn_var", state["final_dwconv_bn.running_var"])
    register("expand_conv_w", state["expand_conv.weight"])
    register("expand_bn_w", state["expand_bn.weight"])
    register("expand_bn_b", state["expand_bn.bias"])
    register("expand_bn_mean", state["expand_bn.running_mean"])
    register("expand_bn_var", state["expand_bn.running_var"])
    register("classifier_w", state["classifier.weight"])
    register("classifier_b", state["classifier.bias"])

    # ================================================================
    # 预计算 scratch buffer 大小和偏移量
    # ================================================================
    KWS_W = 101  # 16000 / 160 + 1

    offsets = {}
    offset = 0

    def alloc_buf(name, size):
        nonlocal offset
        # 8-byte alignment (2 floats)
        offset = (offset + 1) // 2 * 2
        offsets[name] = offset
        offset += size

    # mel + f0
    alloc_buf("mel", KWS_W * config.MEL_BINS)
    alloc_buf("f0", 1 * config.MEL_BINS * KWS_W)

    # head output
    head_h = (config.MEL_BINS + 2 * 2 - 5) // 2 + 1  # 20
    alloc_buf("head_out", config.CHANNELS[0] * head_h * KWS_W)

    # Track H dimension through blocks
    cur_h_val = head_h

    for bi, blk in enumerate(all_blocks):
        out_c = blk["out_c"]
        stride = blk["freq_stride"]

        # DWConv 3x1 output height
        out_h_val = (cur_h_val + 2 * 1 - 3) // stride + 1

        alloc_buf(f"t1_{bi}", out_c * cur_h_val * KWS_W)
        alloc_buf(f"t2_{bi}", out_c * out_h_val * KWS_W)
        alloc_buf(f"texp_{bi}", out_c * out_h_val * KWS_W)
        alloc_buf(f"tout_{bi}", out_c * out_h_val * KWS_W)
        alloc_buf(f"br_p_{bi}", out_c * KWS_W)
        alloc_buf(f"br_d_{bi}", out_c * KWS_W)
        alloc_buf(f"br_o_{bi}", out_c * KWS_W)
        alloc_buf(f"nxt_{bi}", out_c * out_h_val * KWS_W)

        cur_h_val = out_h_val

    # Final layers
    fd_out_h = (cur_h_val + 2 * 2 - 5) // 5 + 1  # should be 1
    alloc_buf("fd_out", config.CHANNELS[4] * fd_out_h * KWS_W)
    alloc_buf("ex_out", config.CHANNELS[5] * KWS_W)

    scratch_size = offset
    print(f"Scratch buffer: {scratch_size} floats = {scratch_size * 4 / 1024:.1f} KB")

    # ================================================================
    # 生成 kws_inference.c
    # ================================================================
    I = INDENT
    c = []  # lines accumulator

    c.append('/* BC-ResNet C 推理 — 由 generate_c_inference.py 自动生成 */')
    c.append('/* float + 静态缓冲区，适配 TMS320C6748 DSP */')
    c.append('#include "kws_inference.h"')
    c.append('#include "kws_weights.h"')
    c.append('#include <math.h>')
    c.append('#include <string.h>')
    c.append('')
    c.append(f'#define KWS_W {KWS_W}')
    c.append(f'#define KWS_SCRATCH_SIZE {scratch_size}')
    c.append('')
    c.append('/* ===== 全局预计算（放入 .far 段 / DDR2） ===== */')
    c.append('#ifdef __TI_COMPILER_VERSION__')
    c.append('#pragma DATA_SECTION(g_window, ".far")')
    c.append('#pragma DATA_SECTION(g_mel_fb, ".far")')
    c.append('#pragma DATA_SECTION(kws_scratch_buf, ".far")')
    c.append('#endif')
    c.append('static float g_window[WIN_SIZE];')
    c.append('static float g_mel_fb[FREQ_NUM * MELS_NUM];')
    c.append('static float kws_scratch_buf[KWS_SCRATCH_SIZE];')
    c.append('')

    # Offsets
    c.append('/* ===== Scratch buffer offsets (in floats) ===== */')
    for name, off in sorted(offsets.items(), key=lambda x: x[1]):
        c.append(f'#define OFS_{name} {off}')
    c.append('')

    # Utility
    c.append('/* ===== 工具 ===== */')
    c.append('static inline float fmaxf_local(float a, float b) { return a > b ? a : b; }')
    c.append('static inline float fminf_local(float a, float b) { return a < b ? a : b; }')
    c.append('')

    # FFT
    c.append('static void my_fft(float* x, float* y, long n, long sign) {')
    c.append(I + 'long i, j, k, m, n1, n2;')
    c.append(I + 'float c, c1, e, s, s1, t, tr, ti;')
    c.append(I + 'for (j = i = 1; i < 31; i++) { m = i; j = 2 * j; if (j == n) break; }')
    c.append(I + 'for (n1 = n - 1, j = 0, i = 0; i < n1; i++) {')
    c.append(I * 2 + 'if (i < j) { tr = x[j]; ti = y[j]; x[j] = x[i]; y[j] = y[i]; x[i] = tr; y[i] = ti; }')
    c.append(I * 2 + 'k = n / 2; while (k < (j + 1)) { j -= k; k /= 2; } j += k; }')
    c.append(I + 'for (n2 = 1; n2 < n; n2 *= 2) {')
    c.append(I * 2 + 'c1 = cosf(PI / n2); s1 = -sign * sinf(PI / n2); n1 = 2 * n2;')
    c.append(I * 2 + 'c = 1.0f; s = 0.0f;')
    c.append(I * 2 + 'for (j = 0; j < n2; j++) {')
    c.append(I * 3 + 'for (i = j; i < n; i += n1) {')
    c.append(I * 4 + 'k = i + n2;')
    c.append(I * 4 + 'tr = c * x[k] - s * y[k]; ti = c * y[k] + s * x[k];')
    c.append(I * 4 + 'x[k] = x[i] - tr; y[k] = y[i] - ti;')
    c.append(I * 4 + 'x[i] += tr; y[i] += ti; }')
    c.append(I * 3 + 't = c; c = c * c1 - s * s1; s = t * s1 + s * c1; } }')
    c.append(I + 'if (sign == -1) for (i = 0; i < n; i++) { x[i] /= n; y[i] /= n; }')
    c.append('}')
    c.append('')

    # kws_init
    c.append('void kws_init(void) {')
    c.append(I + 'long i, j;')
    c.append(I + 'for (i = 0; i < WIN_SIZE; i++)')
    c.append(I * 2 + 'g_window[i] = 0.5f * (1.0f - cosf(2.0f * PI * i / WIN_SIZE));')
    c.append('')
    c.append(I + 'float all_freqs[FREQ_NUM];')
    c.append(I + 'float m_pts[MELS_NUM + 2], f_pts[MELS_NUM + 2], f_diff[MELS_NUM + 1];')
    c.append(I + 'float f_max = FS / 2.0f;')
    c.append(I + 'for (i = 0; i < FREQ_NUM; i++) all_freqs[i] = (float)i * f_max / (FREQ_NUM - 1);')
    c.append(I + 'float m_min = 2595.0f * log10f(1.0f + 0.0f / 700.0f);')
    c.append(I + 'float m_max = 2595.0f * log10f(1.0f + f_max / 700.0f);')
    c.append(I + 'm_pts[0] = f_pts[0] = 0;')
    c.append(I + 'for (i = 0; i <= MELS_NUM; i++) {')
    c.append(I * 2 + 'm_pts[i + 1] = m_min + (m_max - m_min) * (i + 1) / (MELS_NUM + 1);')
    c.append(I * 2 + 'f_pts[i + 1] = 700.0f * (powf(10.0f, m_pts[i + 1] / 2595.0f) - 1.0f); }')
    c.append(I + 'for (i = 0; i < MELS_NUM + 1; i++) f_diff[i] = f_pts[i + 1] - f_pts[i];')
    c.append(I + 'for (i = 0; i < FREQ_NUM; i++)')
    c.append(I * 2 + 'for (j = 0; j < MELS_NUM; j++) {')
    c.append(I * 3 + 'float low = (all_freqs[i] - f_pts[j]) / (f_diff[j] + 1e-12f);')
    c.append(I * 3 + 'float high = (f_pts[j + 2] - all_freqs[i]) / (f_diff[j + 1] + 1e-12f);')
    c.append(I * 3 + 'g_mel_fb[i * MELS_NUM + j] = fmaxf_local(0.0f, fminf_local(low, high)); }')
    c.append('}')
    c.append('')

    # extract_log_mel
    c.append('static void extract_log_mel(const short wf[], int len, float* mel) {')
    c.append(I + 'long i, n, k, m;')
    c.append(I + 'long nf = len / FRM_LEN + 1;')
    c.append(I + 'long pad = WIN_SIZE / 2;')
    c.append(I + 'for (n = 0; n < nf; n++) {')
    c.append(I * 2 + 'float re[FFT_LEN], im[FFT_LEN];')
    c.append(I * 2 + 'for (i = 0; i < FFT_LEN; i++) re[i] = im[i] = 0;')
    c.append(I * 2 + 'for (i = 0; i < WIN_SIZE; i++) {')
    c.append(I * 3 + 'long si = n * FRM_LEN + i - pad;')
    c.append(I * 3 + 'if (si < 0) si = -si;')
    c.append(I * 3 + 'else if (si >= len) si = 2 * len - si - 2;')
    c.append(I * 3 + 're[i] = (wf[si] / 32768.0f) * g_window[i]; }')
    c.append(I * 2 + 'my_fft(re, im, FFT_LEN, 1);')
    c.append(I * 2 + 'for (m = 0; m < MELS_NUM; m++) {')
    c.append(I * 3 + 'float a = 0;')
    c.append(I * 3 + 'for (k = 0; k < FREQ_NUM; k++)')
    c.append(I * 4 + 'a += (re[k]*re[k] + im[k]*im[k]) * g_mel_fb[k * MELS_NUM + m];')
    c.append(I * 3 + 'mel[n * MELS_NUM + m] = logf(a + 1.0e-6f); } }')
    c.append('}')
    c.append('')

    # ================================================================
    # kws_recognize
    # ================================================================
    c.append('int kws_recognize(const short waveform[], int signal_length) {')
    c.append(I + 'long i;')
    c.append(I + 'long W = signal_length / FRM_LEN + 1;')
    c.append(I + 'if (W > KWS_W) return -1;')
    c.append('')

    # Mel feature extraction
    c.append(I + '/* --- Mel 特征 --- */')
    c.append(I + 'float* mel = &kws_scratch_buf[OFS_mel];')
    c.append(I + 'extract_log_mel(waveform, signal_length, mel);')
    c.append('')

    # Transpose to [c][h][w] = [1][MELS_NUM][W]
    c.append(I + 'float* f0 = &kws_scratch_buf[OFS_f0];')
    c.append(I + 'for (long h = 0; h < MELS_NUM; h++)')
    c.append(I * 2 + 'for (long w = 0; w < W; w++)')
    c.append(I * 3 + 'f0[h * W + w] = mel[w * MELS_NUM + h];')
    c.append('')

    # Head Conv
    c.append(I + '/* --- Head Conv: 1x40xW -> 16x20xW --- */')
    c.append(I + 'float* head_out = &kws_scratch_buf[OFS_head_out];')
    head_h = (config.MEL_BINS + 2 * 2 - 5) // 2 + 1
    c.append(I + f'for (long oc = 0; oc < CHANNEL0; oc++) {{')
    c.append(I * 2 + f'for (long oh = 0; oh < {head_h}; oh++) {{')
    c.append(I * 3 + 'for (long ow = 0; ow < W; ow++) {')
    c.append(I * 4 + 'float s = 0;')
    c.append(I * 4 + 'for (long ic = 0; ic < 1; ic++)')
    c.append(I * 5 + 'for (long fh = 0; fh < 5; fh++)')
    c.append(I * 6 + 'for (long fw = 0; fw < 5; fw++) {')
    c.append(I * 7 + 'long ih = oh * 2 + fh - 2;')
    c.append(I * 7 + 'long iw = ow * 1 + fw - 2;')
    c.append(I * 7 + f'if (ih >= 0 && ih < MELS_NUM && iw >= 0 && iw < W)')
    c.append(I * 8 + 's += f0[ih * W + iw] * head_conv_w[((oc * 1 + ic) * 5 + fh) * 5 + fw];')
    c.append(I * 7 + '}')
    c.append(I * 4 + 's = ((s - head_bn_mean[oc]) / sqrtf(head_bn_var[oc] + 1.0e-5f)) * head_bn_w[oc] + head_bn_b[oc];')
    c.append(I * 4 + f'head_out[oc * {head_h} * W + oh * W + ow] = (s > 0) ? s : 0;')
    c.append(I * 3 + '}')
    c.append(I * 2 + '}')
    c.append(I + '}')
    c.append('')

    c.append(I + 'float* cur = head_out;')
    cur_c = config.CHANNELS[0]
    cur_h = head_h
    cur_h_val = head_h
    c.append('')

    # BC-ResBlocks
    for bi, blk in enumerate(all_blocks):
        w = blk["weights"]
        prefix = blk["prefix"]
        in_c = blk["in_c"]
        out_c = blk["out_c"]
        stride = blk["freq_stride"]
        dil = blk["dilation"]
        exp = blk["expansion"]
        skip = blk["use_skip"]

        out_h_val = (cur_h_val + 2 * 1 - 3) // stride + 1

        c.append(f'{I}/* --- {prefix}: {in_c}->{out_c}, H={cur_h_val}->{out_h_val}, dil={dil}, exp={exp} --- */')

        # t1: 1x1 Conv + BN + ReLU
        c.append(f'{I}float* t1_{bi} = &kws_scratch_buf[OFS_t1_{bi}];')
        c.append(f'{I}/* 1x1 Conv */')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++) {{')
        c.append(f'{I}{I}for (long oh = 0; oh < {cur_h_val}; oh++) {{')
        c.append(f'{I}{I}{I}for (long ow = 0; ow < W; ow++) {{')
        c.append(f'{I}{I}{I}{I}float s = 0;')
        c.append(f'{I}{I}{I}{I}for (long ic = 0; ic < {in_c}; ic++)')
        c.append(f'{I}{I}{I}{I}{I}s += cur[ic * {cur_h_val} * W + oh * W + ow] * {w["main_conv1x1_w"]}[oc * {in_c} + ic];')
        c.append(f'{I}{I}{I}{I}s = ((s - {w["main_bn1_mean"]}[oc]) / sqrtf({w["main_bn1_var"]}[oc] + 1.0e-5f)) * {w["main_bn1_w"]}[oc] + {w["main_bn1_b"]}[oc];')
        c.append(f'{I}{I}{I}{I}t1_{bi}[oc * {cur_h_val} * W + oh * W + ow] = (s > 0) ? s : 0;')
        c.append(f'{I}{I}{I}}}')
        c.append(f'{I}{I}}}')
        c.append(f'{I}}}')

        # t2: 3x1 DWConv + BN + ReLU
        c.append(f'{I}/* 3x1 DWConv stride={stride} */')
        c.append(f'{I}float* t2_{bi} = &kws_scratch_buf[OFS_t2_{bi}];')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++) {{')
        c.append(f'{I}{I}for (long oh = 0; oh < {out_h_val}; oh++) {{')
        c.append(f'{I}{I}{I}for (long ow = 0; ow < W; ow++) {{')
        c.append(f'{I}{I}{I}{I}float s = 0;')
        c.append(f'{I}{I}{I}{I}for (long fh = 0; fh < 3; fh++) {{')
        c.append(f'{I}{I}{I}{I}{I}long ih = oh * {stride} + fh - 1;')
        c.append(f'{I}{I}{I}{I}{I}if (ih >= 0 && ih < {cur_h_val})')
        c.append(f'{I}{I}{I}{I}{I}{I}s += t1_{bi}[oc * {cur_h_val} * W + ih * W + ow] * {w["main_dwconv_w"]}[oc * 3 + fh];')
        c.append(f'{I}{I}{I}{I}}}')
        c.append(f'{I}{I}{I}{I}s = ((s - {w["main_bn2_mean"]}[oc]) / sqrtf({w["main_bn2_var"]}[oc] + 1.0e-5f)) * {w["main_bn2_w"]}[oc] + {w["main_bn2_b"]}[oc];')
        c.append(f'{I}{I}{I}{I}t2_{bi}[oc * {out_h_val} * W + oh * W + ow] = (s > 0) ? s : 0;')
        c.append(f'{I}{I}{I}}}')
        c.append(f'{I}{I}}}')
        c.append(f'{I}}}')

        # texp: internal expand reshape + BN
        h_div = out_h_val // exp
        c.append(f'{I}/* Internal expand: ({out_c},{out_h_val},W) -> ({out_c}*{exp},{h_div},W) -> BN -> back */')
        c.append(f'{I}float* texp_{bi} = &kws_scratch_buf[OFS_texp_{bi}];')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++)')
        c.append(f'{I}{I}for (long e = 0; e < {exp}; e++)')
        c.append(f'{I}{I}{I}for (long oh = 0; oh < {h_div}; oh++)')
        c.append(f'{I}{I}{I}{I}for (long ow = 0; ow < W; ow++)')
        c.append(f'{I}{I}{I}{I}{I}texp_{bi}[(oc * {exp} + e) * {h_div} * W + oh * W + ow] = t2_{bi}[oc * {out_h_val} * W + (oh * {exp} + e) * W + ow];')
        c.append(f'{I}for (long ch = 0; ch < {out_c} * {exp}; ch++) {{')
        c.append(f'{I}{I}float sc = {w["expand_bn_w"]}[ch] / sqrtf({w["expand_bn_var"]}[ch] + 1.0e-5f);')
        c.append(f'{I}{I}float sh = {w["expand_bn_b"]}[ch] - {w["expand_bn_mean"]}[ch] * sc;')
        c.append(f'{I}{I}for (long i = 0; i < {h_div} * W; i++)')
        c.append(f'{I}{I}{I}texp_{bi}[ch * {h_div} * W + i] = texp_{bi}[ch * {h_div} * W + i] * sc + sh;')
        c.append(f'{I}}}')
        # tout: reshape back + ReLU
        c.append(f'{I}float* tout_{bi} = &kws_scratch_buf[OFS_tout_{bi}];')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++)')
        c.append(f'{I}{I}for (long e = 0; e < {exp}; e++)')
        c.append(f'{I}{I}{I}for (long oh = 0; oh < {h_div}; oh++)')
        c.append(f'{I}{I}{I}{I}for (long ow = 0; ow < W; ow++) {{')
        c.append(f'{I}{I}{I}{I}{I}float v = texp_{bi}[(oc * {exp} + e) * {h_div} * W + oh * W + ow];')
        c.append(f'{I}{I}{I}{I}{I}tout_{bi}[oc * {out_h_val} * W + (oh * {exp} + e) * W + ow] = (v > 0) ? v : 0;')
        c.append(f'{I}{I}{I}{I}}}')

        # Broadcast residual
        c.append(f'{I}/* Broadcast residual: AvgPool -> 1x3 DWConv dil={dil} -> BN -> Sigmoid -> 1x1 proj */')
        c.append(f'{I}float* br_p_{bi} = &kws_scratch_buf[OFS_br_p_{bi}];')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++)')
        c.append(f'{I}{I}for (long ow = 0; ow < W; ow++) {{')
        c.append(f'{I}{I}{I}float s = 0;')
        c.append(f'{I}{I}{I}for (long ih = 0; ih < {out_h_val}; ih++) s += tout_{bi}[oc * {out_h_val} * W + ih * W + ow];')
        c.append(f'{I}{I}{I}br_p_{bi}[oc * W + ow] = s / {out_h_val};')
        c.append(f'{I}{I}}}')
        # DWConv 1x3 dilated
        c.append(f'{I}float* br_d_{bi} = &kws_scratch_buf[OFS_br_d_{bi}];')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++)')
        c.append(f'{I}{I}for (long ow = 0; ow < W; ow++) {{')
        c.append(f'{I}{I}{I}float s = 0;')
        c.append(f'{I}{I}{I}for (long fw = 0; fw < 3; fw++) {{')
        c.append(f'{I}{I}{I}{I}long iw = ow + (fw - 1) * {dil};')
        c.append(f'{I}{I}{I}{I}if (iw >= 0 && iw < W) s += br_p_{bi}[oc * W + iw] * {w["br_dwconv_w"]}[oc * 3 + fw];')
        c.append(f'{I}{I}{I}}}')
        c.append(f'{I}{I}{I}s = ((s - {w["br_bn_mean"]}[oc]) / sqrtf({w["br_bn_var"]}[oc] + 1.0e-5f)) * {w["br_bn_w"]}[oc] + {w["br_bn_b"]}[oc];')
        c.append(f'{I}{I}{I}br_d_{bi}[oc * W + ow] = 1.0f / (1.0f + expf(-s));')
        c.append(f'{I}{I}}}')
        # 1x1 projection
        c.append(f'{I}float* br_o_{bi} = &kws_scratch_buf[OFS_br_o_{bi}];')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++)')
        c.append(f'{I}{I}for (long ow = 0; ow < W; ow++) {{')
        c.append(f'{I}{I}{I}float s = 0;')
        c.append(f'{I}{I}{I}for (long ic = 0; ic < {out_c}; ic++) s += br_d_{bi}[ic * W + ow] * {w["br_proj_w"]}[oc * {out_c} + ic];')
        c.append(f'{I}{I}{I}br_o_{bi}[oc * W + ow] = s;')
        c.append(f'{I}{I}}}')

        # Add + ReLU (+ skip)
        c.append(f'{I}/* Add + ReLU */')
        c.append(f'{I}float* nxt_{bi} = &kws_scratch_buf[OFS_nxt_{bi}];')
        c.append(f'{I}for (long oc = 0; oc < {out_c}; oc++)')
        c.append(f'{I}{I}for (long oh = 0; oh < {out_h_val}; oh++)')
        c.append(f'{I}{I}{I}for (long ow = 0; ow < W; ow++) {{')
        c.append(f'{I}{I}{I}{I}float v = tout_{bi}[oc * {out_h_val} * W + oh * W + ow] + br_o_{bi}[oc * W + ow];')
        if skip:
            c.append(f'{I}{I}{I}{I}if (oc < {in_c}) {{')
            if in_c == out_c and cur_h_val == out_h_val:
                c.append(f'{I}{I}{I}{I}{I}v += cur[oc * {cur_h_val} * W + oh * W + ow];')
            else:
                c.append(f'{I}{I}{I}{I}{I}/* adaptive pool skip: {cur_h_val}->{out_h_val} */')
                c.append(f'{I}{I}{I}{I}{I}long hs = (long)(oh * {cur_h_val} / (float){out_h_val});')
                c.append(f'{I}{I}{I}{I}{I}long he = (long)((oh + 1) * {cur_h_val} / (float){out_h_val});')
                c.append(f'{I}{I}{I}{I}{I}float ss = 0; long cnt = 0;')
                c.append(f'{I}{I}{I}{I}{I}for (long ih = hs; ih < he; ih++) {{ ss += cur[oc * {cur_h_val} * W + ih * W + ow]; cnt++; }}')
                c.append(f'{I}{I}{I}{I}{I}v += ss / cnt;')
            c.append(f'{I}{I}{I}{I}}}')
        c.append(f'{I}{I}{I}{I}nxt_{bi}[oc * {out_h_val} * W + oh * W + ow] = (v > 0) ? v : 0;')
        c.append(f'{I}{I}{I}}}')
        c.append(f'{I}cur = nxt_{bi};')

        cur_c = out_c
        cur_h_val = out_h_val
        c.append('')

    # Final layers
    c.append(f'{I}/* --- Final DWConv 5x5: ({cur_c},{cur_h_val},W) -> ({cur_c},1,W) --- */')
    fd_out_h = (cur_h_val + 2 * 2 - 5) // 5 + 1
    c.append(f'{I}float* fd_out = &kws_scratch_buf[OFS_fd_out];')
    c.append(f'{I}for (long oc = 0; oc < CHANNEL4; oc++) {{')
    c.append(f'{I}{I}for (long oh = 0; oh < {fd_out_h}; oh++) {{')
    c.append(f'{I}{I}{I}for (long ow = 0; ow < W; ow++) {{')
    c.append(f'{I}{I}{I}{I}float s = 0;')
    c.append(f'{I}{I}{I}{I}for (long fh = 0; fh < 5; fh++) {{')
    c.append(f'{I}{I}{I}{I}{I}for (long fw = 0; fw < 5; fw++) {{')
    c.append(f'{I}{I}{I}{I}{I}{I}long ih = oh * 5 + fh - 2;')
    c.append(f'{I}{I}{I}{I}{I}{I}long iw = ow + fw - 2;')
    c.append(f'{I}{I}{I}{I}{I}{I}if (ih >= 0 && ih < {cur_h_val} && iw >= 0 && iw < W)')
    c.append(f'{I}{I}{I}{I}{I}{I}{I}s += cur[oc * {cur_h_val} * W + ih * W + iw] * final_dwconv_w[(oc * 5 + fh) * 5 + fw];')
    c.append(f'{I}{I}{I}{I}{I}}}')
    c.append(f'{I}{I}{I}{I}}}')
    c.append(f'{I}{I}{I}{I}s = ((s - final_dwconv_bn_mean[oc]) / sqrtf(final_dwconv_bn_var[oc] + 1.0e-5f)) * final_dwconv_bn_w[oc] + final_dwconv_bn_b[oc];')
    c.append(f'{I}{I}{I}{I}fd_out[oc * W + ow] = (s > 0) ? s : 0;')
    c.append(f'{I}{I}{I}}}')
    c.append(f'{I}{I}}}')
    c.append(f'{I}}}')

    # Expand Conv: CHANNEL4 -> CHANNEL5
    c.append(f'{I}/* --- Expand Conv {config.CHANNELS[4]}->{config.CHANNELS[5]} --- */')
    c.append(f'{I}float* ex_out = &kws_scratch_buf[OFS_ex_out];')
    c.append(f'{I}for (long oc = 0; oc < CHANNEL5; oc++) {{')
    c.append(f'{I}{I}for (long ow = 0; ow < W; ow++) {{')
    c.append(f'{I}{I}{I}float s = 0;')
    c.append(f'{I}{I}{I}for (long ic = 0; ic < CHANNEL4; ic++)')
    c.append(f'{I}{I}{I}{I}s += fd_out[ic * W + ow] * expand_conv_w[oc * CHANNEL4 + ic];')
    c.append(f'{I}{I}{I}s = ((s - expand_bn_mean[oc]) / sqrtf(expand_bn_var[oc] + 1.0e-5f)) * expand_bn_w[oc] + expand_bn_b[oc];')
    c.append(f'{I}{I}{I}ex_out[oc * W + ow] = (s > 0) ? s : 0;')
    c.append(f'{I}{I}}}')
    c.append(f'{I}}}')

    # Global AvgPool + Classifier
    c.append(f'{I}/* --- Global AvgPool + Classifier --- */')
    c.append(f'{I}float logits[CLASSES_NUM];')
    c.append(f'{I}for (long oc = 0; oc < CLASSES_NUM; oc++) {{')
    c.append(f'{I}{I}float s = classifier_b[oc];')
    c.append(f'{I}{I}for (long ic = 0; ic < CHANNEL5; ic++) {{')
    c.append(f'{I}{I}{I}float avg = 0;')
    c.append(f'{I}{I}{I}for (long ow = 0; ow < W; ow++) avg += ex_out[ic * W + ow];')
    c.append(f'{I}{I}{I}avg /= W;')
    c.append(f'{I}{I}{I}s += avg * classifier_w[oc * CHANNEL5 + ic];')
    c.append(f'{I}{I}}}')
    c.append(f'{I}{I}logits[oc] = s;')
    c.append(f'{I}}}')

    # Argmax
    c.append(f'{I}/* --- Argmax --- */')
    c.append(f'{I}long best = 0;')
    c.append(f'{I}for (i = 1; i < CLASSES_NUM; i++)')
    c.append(f'{I}{I}if (logits[i] > logits[best]) best = i;')
    c.append(f'{I}return (int)best;')
    c.append('}')
    c.append('')

    # Label names
    c.append('static const char* g_names[CLASSES_NUM] = {')
    c.append(I + '"_silence_", "_unknown_", "down", "go", "left",')
    c.append(I + '"no", "off", "on", "right", "stop", "up", "yes"')
    c.append('};')
    c.append('')
    c.append('const char* kws_label_name(int idx) {')
    c.append(I + 'if (idx < 0 || idx >= CLASSES_NUM) return "???";')
    c.append(I + 'return g_names[idx];')
    c.append('}')

    # Write file
    output_dir = Path(output_dir)
    c_path = output_dir / "kws_inference.c"
    with open(c_path, "w", encoding="utf-8") as f:
        f.write("\n".join(c))

    print(f"C 推理代码已生成: {c_path}  ({c_path.stat().st_size / 1024:.1f} KB)")
    return c_path


def main():
    print("加载模型...")
    model = BCResNet(
        num_classes=config.NUM_CLASSES, mel_bins=config.MEL_BINS,
        channels=config.CHANNELS, stage_configs=config.STAGE_CONFIGS,
    )
    checkpoint = torch.load(
        config.CHECKPOINT_DIR / "best_model.pth",
        map_location="cpu", weights_only=False,
    )
    model.load_state_dict(checkpoint["model_state_dict"])
    model.eval()

    generate_c_code(model, config.OUTPUT_DIR)
    print("完成。")


if __name__ == "__main__":
    main()
