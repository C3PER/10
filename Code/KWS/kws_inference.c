/* BC-ResNet C 推理 — 由 generate_c_inference.py 自动生成 */
/* float + 静态缓冲区，适配 TMS320C6748 DSP */
#include "kws_inference.h"
#include "kws_weights.h"
#include <math.h>
#include <string.h>

#define KWS_W 101
#define KWS_SCRATCH_SIZE 769028  /* 768004 + FFT_LEN*2 for FFT temp */

/* ===== 全局预计算（放入 .far 段 / DDR2） ===== */
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION(g_window, ".far")
#pragma DATA_SECTION(g_mel_fb, ".far")
#pragma DATA_SECTION(kws_scratch_buf, ".far")
#endif
static float g_window[WIN_SIZE];
static float g_mel_fb[FREQ_NUM * MELS_NUM];
static float g_fft_cos[FFT_LEN / 2];  /* FFT twiddle factor cos table (indexed by n2) */
static float g_fft_sin[FFT_LEN / 2];  /* FFT twiddle factor sin table */
static float g_last_conf = 0;         /* 上次推理的置信度 */
static float g_last_logits[CLASSES_NUM]; /* 上次推理的 softmax 概率 */
static float kws_scratch_buf[KWS_SCRATCH_SIZE];

/* ===== Scratch buffer offsets (in floats) ===== */
#define OFS_mel 0
#define OFS_f0 4040
#define OFS_head_out 8080
#define OFS_t1_0 40400
#define OFS_t2_0 56560
#define OFS_texp_0 72720
#define OFS_tout_0 88880
#define OFS_br_p_0 105040
#define OFS_br_d_0 105848
#define OFS_br_o_0 106656
#define OFS_nxt_0 107464
#define OFS_t1_1 123624
#define OFS_t2_1 139784
#define OFS_texp_1 155944
#define OFS_tout_1 172104
#define OFS_br_p_1 188264
#define OFS_br_d_1 189072
#define OFS_br_o_1 189880
#define OFS_nxt_1 190688
#define OFS_t1_2 206848
#define OFS_t2_2 231088
#define OFS_texp_2 243208
#define OFS_tout_2 255328
#define OFS_br_p_2 267448
#define OFS_br_d_2 268660
#define OFS_br_o_2 269872
#define OFS_nxt_2 271084
#define OFS_t1_3 283204
#define OFS_t2_3 295324
#define OFS_texp_3 307444
#define OFS_tout_3 319564
#define OFS_br_p_3 331684
#define OFS_br_d_3 332896
#define OFS_br_o_3 334108
#define OFS_nxt_3 335320
#define OFS_t1_4 347440
#define OFS_t2_4 363600
#define OFS_texp_4 371680
#define OFS_tout_4 379760
#define OFS_br_p_4 387840
#define OFS_br_d_4 389456
#define OFS_br_o_4 391072
#define OFS_nxt_4 392688
#define OFS_t1_5 400768
#define OFS_t2_5 408848
#define OFS_texp_5 416928
#define OFS_tout_5 425008
#define OFS_br_p_5 433088
#define OFS_br_d_5 434704
#define OFS_br_o_5 436320
#define OFS_nxt_5 437936
#define OFS_t1_6 446016
#define OFS_t2_6 454096
#define OFS_texp_6 462176
#define OFS_tout_6 470256
#define OFS_br_p_6 478336
#define OFS_br_d_6 479952
#define OFS_br_o_6 481568
#define OFS_nxt_6 483184
#define OFS_t1_7 491264
#define OFS_t2_7 499344
#define OFS_texp_7 507424
#define OFS_tout_7 515504
#define OFS_br_p_7 523584
#define OFS_br_d_7 525200
#define OFS_br_o_7 526816
#define OFS_nxt_7 528432
#define OFS_t1_8 536512
#define OFS_t2_8 546612
#define OFS_texp_8 556712
#define OFS_tout_8 566812
#define OFS_br_p_8 576912
#define OFS_br_d_8 578932
#define OFS_br_o_8 580952
#define OFS_nxt_8 582972
#define OFS_t1_9 593072
#define OFS_t2_9 603172
#define OFS_texp_9 613272
#define OFS_tout_9 623372
#define OFS_br_p_9 633472
#define OFS_br_d_9 635492
#define OFS_br_o_9 637512
#define OFS_nxt_9 639532
#define OFS_t1_10 649632
#define OFS_t2_10 659732
#define OFS_texp_10 669832
#define OFS_tout_10 679932
#define OFS_br_p_10 690032
#define OFS_br_d_10 692052
#define OFS_br_o_10 694072
#define OFS_nxt_10 696092
#define OFS_t1_11 706192
#define OFS_t2_11 716292
#define OFS_texp_11 726392
#define OFS_tout_11 736492
#define OFS_br_p_11 746592
#define OFS_br_d_11 748612
#define OFS_br_o_11 750632
#define OFS_nxt_11 752652
#define OFS_fd_out 762752
#define OFS_ex_out 764772
#define OFS_fft_tmp 768004  /* FFT re/im temp (FFT_LEN*2 floats) */

/* ===== 工具 ===== */
static inline float fmaxf_local(float a, float b) { return a > b ? a : b; }
static inline float fminf_local(float a, float b) { return a < b ? a : b; }

static void my_fft(float* x, float* y, long n, long sign) {
    long i, j, k, n1, n2;
    float c, c1, s, s1, t, tr, ti;
    for (j = i = 1; i < 31; i++) { j = 2 * j; if (j == n) break; }
    for (n1 = n - 1, j = 0, i = 0; i < n1; i++) {
        if (i < j) { tr = x[j]; ti = y[j]; x[j] = x[i]; y[j] = y[i]; x[i] = tr; y[i] = ti; }
        k = n / 2; while (k < (j + 1)) { j -= k; k /= 2; } j += k; }
    for (n2 = 1; n2 < n; n2 *= 2) {
        c1 = g_fft_cos[n2]; s1 = -sign * g_fft_sin[n2]; n1 = 2 * n2;
        c = 1.0f; s = 0.0f;
        for (j = 0; j < n2; j++) {
            for (i = j; i < n; i += n1) {
                k = i + n2;
                tr = c * x[k] - s * y[k]; ti = c * y[k] + s * x[k];
                x[k] = x[i] - tr; y[k] = y[i] - ti;
                x[i] += tr; y[i] += ti; }
            t = c; c = c * c1 - s * s1; s = t * s1 + s * c1; } }
    if (sign == -1) for (i = 0; i < n; i++) { x[i] /= n; y[i] /= n; }
}

void kws_init(void) {
    long i, j, n2;
    for (i = 0; i < WIN_SIZE; i++)
        g_window[i] = 0.5f * (1.0f - cosf(2.0f * PI * i / WIN_SIZE));

    /* FFT 旋转因子预计算 */
    for (n2 = 1; n2 < FFT_LEN; n2 *= 2) {
        g_fft_cos[n2] = cosf(PI / n2);
        g_fft_sin[n2] = sinf(PI / n2);
    }

    float all_freqs[FREQ_NUM];
    float m_pts[MELS_NUM + 2], f_pts[MELS_NUM + 2], f_diff[MELS_NUM + 1];
    float f_max = FS / 2.0f;
    for (i = 0; i < FREQ_NUM; i++) all_freqs[i] = (float)i * f_max / (FREQ_NUM - 1);
    float m_min = 2595.0f * log10f(1.0f + 0.0f / 700.0f);
    float m_max = 2595.0f * log10f(1.0f + f_max / 700.0f);
    m_pts[0] = f_pts[0] = 0;
    for (i = 0; i <= MELS_NUM; i++) {
        m_pts[i + 1] = m_min + (m_max - m_min) * (i + 1) / (MELS_NUM + 1);
        f_pts[i + 1] = 700.0f * (powf(10.0f, m_pts[i + 1] / 2595.0f) - 1.0f); }
    for (i = 0; i < MELS_NUM + 1; i++) f_diff[i] = f_pts[i + 1] - f_pts[i];
    for (i = 0; i < FREQ_NUM; i++)
        for (j = 0; j < MELS_NUM; j++) {
            float low = (all_freqs[i] - f_pts[j]) / (f_diff[j] + 1e-12f);
            float high = (f_pts[j + 2] - all_freqs[i]) / (f_diff[j + 1] + 1e-12f);
            g_mel_fb[i * MELS_NUM + j] = fmaxf_local(0.0f, fminf_local(low, high)); }
}

static void extract_log_mel(const short wf[], int len, float* mel) {
    long i, n, k, m;
    long nf = len / FRM_LEN + 1;
    long pad = WIN_SIZE / 2;
    for (n = 0; n < nf; n++) {
        float* re = &kws_scratch_buf[OFS_fft_tmp];
        float* im = &kws_scratch_buf[OFS_fft_tmp + FFT_LEN];
        for (i = 0; i < FFT_LEN; i++) re[i] = im[i] = 0;
        for (i = 0; i < WIN_SIZE; i++) {
            long si = n * FRM_LEN + i - pad;
            if (si < 0) si = -si;
            else if (si >= len) si = 2 * len - si - 2;
            re[i] = (wf[si] / 32768.0f) * g_window[i]; }
        my_fft(re, im, FFT_LEN, 1);
        for (m = 0; m < MELS_NUM; m++) {
            float a = 0;
            for (k = 0; k < FREQ_NUM; k++)
                a += (re[k]*re[k] + im[k]*im[k]) * g_mel_fb[k * MELS_NUM + m];
            mel[n * MELS_NUM + m] = logf(a + 1.0e-6f); } }
}

int kws_recognize(const short waveform[], int signal_length) {
    long i, h, w, oc, oh, ow, ic, fh, fw, ih, e, ch;
    long W = signal_length / FRM_LEN + 1;
    if (W > KWS_W) return -1;

    /* --- Mel 特征 --- */
    float* mel = &kws_scratch_buf[OFS_mel];
    extract_log_mel(waveform, signal_length, mel);

    float* f0 = &kws_scratch_buf[OFS_f0];
    for (h = 0; h < MELS_NUM; h++)
        for (w = 0; w < W; w++)
            f0[h * W + w] = mel[w * MELS_NUM + h];

    /* --- Head Conv: 1x40xW -> 16x20xW --- */
    float* head_out = &kws_scratch_buf[OFS_head_out];
    for (oc = 0; oc < CHANNEL0; oc++) {
        for (oh = 0; oh < 20; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 1; ic++)
                    for (fh = 0; fh < 5; fh++)
                        for (fw = 0; fw < 5; fw++) {
                            long ih = oh * 2 + fh - 2;
                            long iw = ow * 1 + fw - 2;
                            if (ih >= 0 && ih < MELS_NUM && iw >= 0 && iw < W)
                                s += f0[ih * W + iw] * head_conv_w[((oc * 1 + ic) * 5 + fh) * 5 + fw];
                            }
                s = ((s - head_bn_mean[oc]) / sqrtf(head_bn_var[oc] + 1.0e-5f)) * head_bn_w[oc] + head_bn_b[oc];
                head_out[oc * 20 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }

    float* cur = head_out;

    /* --- stage0_block0: 16->8, H=20->20, dil=1, exp=5 --- */
    float* t1_0 = &kws_scratch_buf[OFS_t1_0];
    /* 1x1 Conv */
    for (oc = 0; oc < 8; oc++) {
        for (oh = 0; oh < 20; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 16; ic++)
                    s += cur[ic * 20 * W + oh * W + ow] * stage0_block0_main_conv1x1_w[oc * 16 + ic];
                s = ((s - stage0_block0_main_bn1_mean[oc]) / sqrtf(stage0_block0_main_bn1_var[oc] + 1.0e-5f)) * stage0_block0_main_bn1_w[oc] + stage0_block0_main_bn1_b[oc];
                t1_0[oc * 20 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_0 = &kws_scratch_buf[OFS_t2_0];
    for (oc = 0; oc < 8; oc++) {
        for (oh = 0; oh < 20; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 20)
                        s += t1_0[oc * 20 * W + ih * W + ow] * stage0_block0_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage0_block0_main_bn2_mean[oc]) / sqrtf(stage0_block0_main_bn2_var[oc] + 1.0e-5f)) * stage0_block0_main_bn2_w[oc] + stage0_block0_main_bn2_b[oc];
                t2_0[oc * 20 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (8,20,W) -> (8*5,4,W) -> BN -> back */
    float* texp_0 = &kws_scratch_buf[OFS_texp_0];
    for (oc = 0; oc < 8; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 4; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_0[(oc * 5 + e) * 4 * W + oh * W + ow] = t2_0[oc * 20 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 8 * 5; ch++) {
        float sc = stage0_block0_expand_bn_w[ch] / sqrtf(stage0_block0_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage0_block0_expand_bn_b[ch] - stage0_block0_expand_bn_mean[ch] * sc;
        for (i = 0; i < 4 * W; i++)
            texp_0[ch * 4 * W + i] = texp_0[ch * 4 * W + i] * sc + sh;
    }
    float* tout_0 = &kws_scratch_buf[OFS_tout_0];
    for (oc = 0; oc < 8; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 4; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_0[(oc * 5 + e) * 4 * W + oh * W + ow];
                    tout_0[oc * 20 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=1 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_0 = &kws_scratch_buf[OFS_br_p_0];
    for (oc = 0; oc < 8; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 20; ih++) s += tout_0[oc * 20 * W + ih * W + ow];
            br_p_0[oc * W + ow] = s / 20;
        }
    float* br_d_0 = &kws_scratch_buf[OFS_br_d_0];
    for (oc = 0; oc < 8; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 1;
                if (iw >= 0 && iw < W) s += br_p_0[oc * W + iw] * stage0_block0_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage0_block0_br_bn_mean[oc]) / sqrtf(stage0_block0_br_bn_var[oc] + 1.0e-5f)) * stage0_block0_br_bn_w[oc] + stage0_block0_br_bn_b[oc];
            br_d_0[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_0 = &kws_scratch_buf[OFS_br_o_0];
    for (oc = 0; oc < 8; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 8; ic++) s += br_d_0[ic * W + ow] * stage0_block0_br_proj_w[oc * 8 + ic];
            br_o_0[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_0 = &kws_scratch_buf[OFS_nxt_0];
    for (oc = 0; oc < 8; oc++)
        for (oh = 0; oh < 20; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_0[oc * 20 * W + oh * W + ow] + br_o_0[oc * W + ow];
                nxt_0[oc * 20 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_0;

    /* --- stage0_block1: 8->8, H=20->20, dil=1, exp=5 --- */
    float* t1_1 = &kws_scratch_buf[OFS_t1_1];
    /* 1x1 Conv */
    for (oc = 0; oc < 8; oc++) {
        for (oh = 0; oh < 20; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 8; ic++)
                    s += cur[ic * 20 * W + oh * W + ow] * stage0_block1_main_conv1x1_w[oc * 8 + ic];
                s = ((s - stage0_block1_main_bn1_mean[oc]) / sqrtf(stage0_block1_main_bn1_var[oc] + 1.0e-5f)) * stage0_block1_main_bn1_w[oc] + stage0_block1_main_bn1_b[oc];
                t1_1[oc * 20 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_1 = &kws_scratch_buf[OFS_t2_1];
    for (oc = 0; oc < 8; oc++) {
        for (oh = 0; oh < 20; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 20)
                        s += t1_1[oc * 20 * W + ih * W + ow] * stage0_block1_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage0_block1_main_bn2_mean[oc]) / sqrtf(stage0_block1_main_bn2_var[oc] + 1.0e-5f)) * stage0_block1_main_bn2_w[oc] + stage0_block1_main_bn2_b[oc];
                t2_1[oc * 20 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (8,20,W) -> (8*5,4,W) -> BN -> back */
    float* texp_1 = &kws_scratch_buf[OFS_texp_1];
    for (oc = 0; oc < 8; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 4; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_1[(oc * 5 + e) * 4 * W + oh * W + ow] = t2_1[oc * 20 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 8 * 5; ch++) {
        float sc = stage0_block1_expand_bn_w[ch] / sqrtf(stage0_block1_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage0_block1_expand_bn_b[ch] - stage0_block1_expand_bn_mean[ch] * sc;
        for (i = 0; i < 4 * W; i++)
            texp_1[ch * 4 * W + i] = texp_1[ch * 4 * W + i] * sc + sh;
    }
    float* tout_1 = &kws_scratch_buf[OFS_tout_1];
    for (oc = 0; oc < 8; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 4; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_1[(oc * 5 + e) * 4 * W + oh * W + ow];
                    tout_1[oc * 20 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=1 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_1 = &kws_scratch_buf[OFS_br_p_1];
    for (oc = 0; oc < 8; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 20; ih++) s += tout_1[oc * 20 * W + ih * W + ow];
            br_p_1[oc * W + ow] = s / 20;
        }
    float* br_d_1 = &kws_scratch_buf[OFS_br_d_1];
    for (oc = 0; oc < 8; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 1;
                if (iw >= 0 && iw < W) s += br_p_1[oc * W + iw] * stage0_block1_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage0_block1_br_bn_mean[oc]) / sqrtf(stage0_block1_br_bn_var[oc] + 1.0e-5f)) * stage0_block1_br_bn_w[oc] + stage0_block1_br_bn_b[oc];
            br_d_1[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_1 = &kws_scratch_buf[OFS_br_o_1];
    for (oc = 0; oc < 8; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 8; ic++) s += br_d_1[ic * W + ow] * stage0_block1_br_proj_w[oc * 8 + ic];
            br_o_1[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_1 = &kws_scratch_buf[OFS_nxt_1];
    for (oc = 0; oc < 8; oc++)
        for (oh = 0; oh < 20; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_1[oc * 20 * W + oh * W + ow] + br_o_1[oc * W + ow];
                if (oc < 8) {
                    v += cur[oc * 20 * W + oh * W + ow];
                }
                nxt_1[oc * 20 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_1;

    /* --- stage1_block0: 8->12, H=20->10, dil=1, exp=5 --- */
    float* t1_2 = &kws_scratch_buf[OFS_t1_2];
    /* 1x1 Conv */
    for (oc = 0; oc < 12; oc++) {
        for (oh = 0; oh < 20; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 8; ic++)
                    s += cur[ic * 20 * W + oh * W + ow] * stage1_block0_main_conv1x1_w[oc * 8 + ic];
                s = ((s - stage1_block0_main_bn1_mean[oc]) / sqrtf(stage1_block0_main_bn1_var[oc] + 1.0e-5f)) * stage1_block0_main_bn1_w[oc] + stage1_block0_main_bn1_b[oc];
                t1_2[oc * 20 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=2 */
    float* t2_2 = &kws_scratch_buf[OFS_t2_2];
    for (oc = 0; oc < 12; oc++) {
        for (oh = 0; oh < 10; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 2 + fh - 1;
                    if (ih >= 0 && ih < 20)
                        s += t1_2[oc * 20 * W + ih * W + ow] * stage1_block0_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage1_block0_main_bn2_mean[oc]) / sqrtf(stage1_block0_main_bn2_var[oc] + 1.0e-5f)) * stage1_block0_main_bn2_w[oc] + stage1_block0_main_bn2_b[oc];
                t2_2[oc * 10 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (12,10,W) -> (12*5,2,W) -> BN -> back */
    float* texp_2 = &kws_scratch_buf[OFS_texp_2];
    for (oc = 0; oc < 12; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 2; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_2[(oc * 5 + e) * 2 * W + oh * W + ow] = t2_2[oc * 10 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 12 * 5; ch++) {
        float sc = stage1_block0_expand_bn_w[ch] / sqrtf(stage1_block0_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage1_block0_expand_bn_b[ch] - stage1_block0_expand_bn_mean[ch] * sc;
        for (i = 0; i < 2 * W; i++)
            texp_2[ch * 2 * W + i] = texp_2[ch * 2 * W + i] * sc + sh;
    }
    float* tout_2 = &kws_scratch_buf[OFS_tout_2];
    for (oc = 0; oc < 12; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 2; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_2[(oc * 5 + e) * 2 * W + oh * W + ow];
                    tout_2[oc * 10 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=1 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_2 = &kws_scratch_buf[OFS_br_p_2];
    for (oc = 0; oc < 12; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 10; ih++) s += tout_2[oc * 10 * W + ih * W + ow];
            br_p_2[oc * W + ow] = s / 10;
        }
    float* br_d_2 = &kws_scratch_buf[OFS_br_d_2];
    for (oc = 0; oc < 12; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 1;
                if (iw >= 0 && iw < W) s += br_p_2[oc * W + iw] * stage1_block0_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage1_block0_br_bn_mean[oc]) / sqrtf(stage1_block0_br_bn_var[oc] + 1.0e-5f)) * stage1_block0_br_bn_w[oc] + stage1_block0_br_bn_b[oc];
            br_d_2[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_2 = &kws_scratch_buf[OFS_br_o_2];
    for (oc = 0; oc < 12; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 12; ic++) s += br_d_2[ic * W + ow] * stage1_block0_br_proj_w[oc * 12 + ic];
            br_o_2[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_2 = &kws_scratch_buf[OFS_nxt_2];
    for (oc = 0; oc < 12; oc++)
        for (oh = 0; oh < 10; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_2[oc * 10 * W + oh * W + ow] + br_o_2[oc * W + ow];
                nxt_2[oc * 10 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_2;

    /* --- stage1_block1: 12->12, H=10->10, dil=1, exp=5 --- */
    float* t1_3 = &kws_scratch_buf[OFS_t1_3];
    /* 1x1 Conv */
    for (oc = 0; oc < 12; oc++) {
        for (oh = 0; oh < 10; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 12; ic++)
                    s += cur[ic * 10 * W + oh * W + ow] * stage1_block1_main_conv1x1_w[oc * 12 + ic];
                s = ((s - stage1_block1_main_bn1_mean[oc]) / sqrtf(stage1_block1_main_bn1_var[oc] + 1.0e-5f)) * stage1_block1_main_bn1_w[oc] + stage1_block1_main_bn1_b[oc];
                t1_3[oc * 10 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_3 = &kws_scratch_buf[OFS_t2_3];
    for (oc = 0; oc < 12; oc++) {
        for (oh = 0; oh < 10; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 10)
                        s += t1_3[oc * 10 * W + ih * W + ow] * stage1_block1_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage1_block1_main_bn2_mean[oc]) / sqrtf(stage1_block1_main_bn2_var[oc] + 1.0e-5f)) * stage1_block1_main_bn2_w[oc] + stage1_block1_main_bn2_b[oc];
                t2_3[oc * 10 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (12,10,W) -> (12*5,2,W) -> BN -> back */
    float* texp_3 = &kws_scratch_buf[OFS_texp_3];
    for (oc = 0; oc < 12; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 2; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_3[(oc * 5 + e) * 2 * W + oh * W + ow] = t2_3[oc * 10 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 12 * 5; ch++) {
        float sc = stage1_block1_expand_bn_w[ch] / sqrtf(stage1_block1_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage1_block1_expand_bn_b[ch] - stage1_block1_expand_bn_mean[ch] * sc;
        for (i = 0; i < 2 * W; i++)
            texp_3[ch * 2 * W + i] = texp_3[ch * 2 * W + i] * sc + sh;
    }
    float* tout_3 = &kws_scratch_buf[OFS_tout_3];
    for (oc = 0; oc < 12; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 2; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_3[(oc * 5 + e) * 2 * W + oh * W + ow];
                    tout_3[oc * 10 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=1 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_3 = &kws_scratch_buf[OFS_br_p_3];
    for (oc = 0; oc < 12; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 10; ih++) s += tout_3[oc * 10 * W + ih * W + ow];
            br_p_3[oc * W + ow] = s / 10;
        }
    float* br_d_3 = &kws_scratch_buf[OFS_br_d_3];
    for (oc = 0; oc < 12; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 1;
                if (iw >= 0 && iw < W) s += br_p_3[oc * W + iw] * stage1_block1_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage1_block1_br_bn_mean[oc]) / sqrtf(stage1_block1_br_bn_var[oc] + 1.0e-5f)) * stage1_block1_br_bn_w[oc] + stage1_block1_br_bn_b[oc];
            br_d_3[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_3 = &kws_scratch_buf[OFS_br_o_3];
    for (oc = 0; oc < 12; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 12; ic++) s += br_d_3[ic * W + ow] * stage1_block1_br_proj_w[oc * 12 + ic];
            br_o_3[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_3 = &kws_scratch_buf[OFS_nxt_3];
    for (oc = 0; oc < 12; oc++)
        for (oh = 0; oh < 10; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_3[oc * 10 * W + oh * W + ow] + br_o_3[oc * W + ow];
                if (oc < 12) {
                    v += cur[oc * 10 * W + oh * W + ow];
                }
                nxt_3[oc * 10 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_3;

    /* --- stage2_block0: 12->16, H=10->5, dil=2, exp=5 --- */
    float* t1_4 = &kws_scratch_buf[OFS_t1_4];
    /* 1x1 Conv */
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 10; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 12; ic++)
                    s += cur[ic * 10 * W + oh * W + ow] * stage2_block0_main_conv1x1_w[oc * 12 + ic];
                s = ((s - stage2_block0_main_bn1_mean[oc]) / sqrtf(stage2_block0_main_bn1_var[oc] + 1.0e-5f)) * stage2_block0_main_bn1_w[oc] + stage2_block0_main_bn1_b[oc];
                t1_4[oc * 10 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=2 */
    float* t2_4 = &kws_scratch_buf[OFS_t2_4];
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 2 + fh - 1;
                    if (ih >= 0 && ih < 10)
                        s += t1_4[oc * 10 * W + ih * W + ow] * stage2_block0_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage2_block0_main_bn2_mean[oc]) / sqrtf(stage2_block0_main_bn2_var[oc] + 1.0e-5f)) * stage2_block0_main_bn2_w[oc] + stage2_block0_main_bn2_b[oc];
                t2_4[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (16,5,W) -> (16*5,1,W) -> BN -> back */
    float* texp_4 = &kws_scratch_buf[OFS_texp_4];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_4[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_4[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 16 * 5; ch++) {
        float sc = stage2_block0_expand_bn_w[ch] / sqrtf(stage2_block0_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage2_block0_expand_bn_b[ch] - stage2_block0_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_4[ch * 1 * W + i] = texp_4[ch * 1 * W + i] * sc + sh;
    }
    float* tout_4 = &kws_scratch_buf[OFS_tout_4];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_4[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_4[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=2 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_4 = &kws_scratch_buf[OFS_br_p_4];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_4[oc * 5 * W + ih * W + ow];
            br_p_4[oc * W + ow] = s / 5;
        }
    float* br_d_4 = &kws_scratch_buf[OFS_br_d_4];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 2;
                if (iw >= 0 && iw < W) s += br_p_4[oc * W + iw] * stage2_block0_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage2_block0_br_bn_mean[oc]) / sqrtf(stage2_block0_br_bn_var[oc] + 1.0e-5f)) * stage2_block0_br_bn_w[oc] + stage2_block0_br_bn_b[oc];
            br_d_4[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_4 = &kws_scratch_buf[OFS_br_o_4];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 16; ic++) s += br_d_4[ic * W + ow] * stage2_block0_br_proj_w[oc * 16 + ic];
            br_o_4[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_4 = &kws_scratch_buf[OFS_nxt_4];
    for (oc = 0; oc < 16; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_4[oc * 5 * W + oh * W + ow] + br_o_4[oc * W + ow];
                nxt_4[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_4;

    /* --- stage2_block1: 16->16, H=5->5, dil=2, exp=5 --- */
    float* t1_5 = &kws_scratch_buf[OFS_t1_5];
    /* 1x1 Conv */
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 16; ic++)
                    s += cur[ic * 5 * W + oh * W + ow] * stage2_block1_main_conv1x1_w[oc * 16 + ic];
                s = ((s - stage2_block1_main_bn1_mean[oc]) / sqrtf(stage2_block1_main_bn1_var[oc] + 1.0e-5f)) * stage2_block1_main_bn1_w[oc] + stage2_block1_main_bn1_b[oc];
                t1_5[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_5 = &kws_scratch_buf[OFS_t2_5];
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 5)
                        s += t1_5[oc * 5 * W + ih * W + ow] * stage2_block1_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage2_block1_main_bn2_mean[oc]) / sqrtf(stage2_block1_main_bn2_var[oc] + 1.0e-5f)) * stage2_block1_main_bn2_w[oc] + stage2_block1_main_bn2_b[oc];
                t2_5[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (16,5,W) -> (16*5,1,W) -> BN -> back */
    float* texp_5 = &kws_scratch_buf[OFS_texp_5];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_5[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_5[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 16 * 5; ch++) {
        float sc = stage2_block1_expand_bn_w[ch] / sqrtf(stage2_block1_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage2_block1_expand_bn_b[ch] - stage2_block1_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_5[ch * 1 * W + i] = texp_5[ch * 1 * W + i] * sc + sh;
    }
    float* tout_5 = &kws_scratch_buf[OFS_tout_5];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_5[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_5[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=2 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_5 = &kws_scratch_buf[OFS_br_p_5];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_5[oc * 5 * W + ih * W + ow];
            br_p_5[oc * W + ow] = s / 5;
        }
    float* br_d_5 = &kws_scratch_buf[OFS_br_d_5];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 2;
                if (iw >= 0 && iw < W) s += br_p_5[oc * W + iw] * stage2_block1_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage2_block1_br_bn_mean[oc]) / sqrtf(stage2_block1_br_bn_var[oc] + 1.0e-5f)) * stage2_block1_br_bn_w[oc] + stage2_block1_br_bn_b[oc];
            br_d_5[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_5 = &kws_scratch_buf[OFS_br_o_5];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 16; ic++) s += br_d_5[ic * W + ow] * stage2_block1_br_proj_w[oc * 16 + ic];
            br_o_5[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_5 = &kws_scratch_buf[OFS_nxt_5];
    for (oc = 0; oc < 16; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_5[oc * 5 * W + oh * W + ow] + br_o_5[oc * W + ow];
                if (oc < 16) {
                    v += cur[oc * 5 * W + oh * W + ow];
                }
                nxt_5[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_5;

    /* --- stage2_block2: 16->16, H=5->5, dil=2, exp=5 --- */
    float* t1_6 = &kws_scratch_buf[OFS_t1_6];
    /* 1x1 Conv */
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 16; ic++)
                    s += cur[ic * 5 * W + oh * W + ow] * stage2_block2_main_conv1x1_w[oc * 16 + ic];
                s = ((s - stage2_block2_main_bn1_mean[oc]) / sqrtf(stage2_block2_main_bn1_var[oc] + 1.0e-5f)) * stage2_block2_main_bn1_w[oc] + stage2_block2_main_bn1_b[oc];
                t1_6[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_6 = &kws_scratch_buf[OFS_t2_6];
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 5)
                        s += t1_6[oc * 5 * W + ih * W + ow] * stage2_block2_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage2_block2_main_bn2_mean[oc]) / sqrtf(stage2_block2_main_bn2_var[oc] + 1.0e-5f)) * stage2_block2_main_bn2_w[oc] + stage2_block2_main_bn2_b[oc];
                t2_6[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (16,5,W) -> (16*5,1,W) -> BN -> back */
    float* texp_6 = &kws_scratch_buf[OFS_texp_6];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_6[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_6[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 16 * 5; ch++) {
        float sc = stage2_block2_expand_bn_w[ch] / sqrtf(stage2_block2_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage2_block2_expand_bn_b[ch] - stage2_block2_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_6[ch * 1 * W + i] = texp_6[ch * 1 * W + i] * sc + sh;
    }
    float* tout_6 = &kws_scratch_buf[OFS_tout_6];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_6[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_6[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=2 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_6 = &kws_scratch_buf[OFS_br_p_6];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_6[oc * 5 * W + ih * W + ow];
            br_p_6[oc * W + ow] = s / 5;
        }
    float* br_d_6 = &kws_scratch_buf[OFS_br_d_6];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 2;
                if (iw >= 0 && iw < W) s += br_p_6[oc * W + iw] * stage2_block2_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage2_block2_br_bn_mean[oc]) / sqrtf(stage2_block2_br_bn_var[oc] + 1.0e-5f)) * stage2_block2_br_bn_w[oc] + stage2_block2_br_bn_b[oc];
            br_d_6[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_6 = &kws_scratch_buf[OFS_br_o_6];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 16; ic++) s += br_d_6[ic * W + ow] * stage2_block2_br_proj_w[oc * 16 + ic];
            br_o_6[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_6 = &kws_scratch_buf[OFS_nxt_6];
    for (oc = 0; oc < 16; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_6[oc * 5 * W + oh * W + ow] + br_o_6[oc * W + ow];
                if (oc < 16) {
                    v += cur[oc * 5 * W + oh * W + ow];
                }
                nxt_6[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_6;

    /* --- stage2_block3: 16->16, H=5->5, dil=2, exp=5 --- */
    float* t1_7 = &kws_scratch_buf[OFS_t1_7];
    /* 1x1 Conv */
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 16; ic++)
                    s += cur[ic * 5 * W + oh * W + ow] * stage2_block3_main_conv1x1_w[oc * 16 + ic];
                s = ((s - stage2_block3_main_bn1_mean[oc]) / sqrtf(stage2_block3_main_bn1_var[oc] + 1.0e-5f)) * stage2_block3_main_bn1_w[oc] + stage2_block3_main_bn1_b[oc];
                t1_7[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_7 = &kws_scratch_buf[OFS_t2_7];
    for (oc = 0; oc < 16; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 5)
                        s += t1_7[oc * 5 * W + ih * W + ow] * stage2_block3_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage2_block3_main_bn2_mean[oc]) / sqrtf(stage2_block3_main_bn2_var[oc] + 1.0e-5f)) * stage2_block3_main_bn2_w[oc] + stage2_block3_main_bn2_b[oc];
                t2_7[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (16,5,W) -> (16*5,1,W) -> BN -> back */
    float* texp_7 = &kws_scratch_buf[OFS_texp_7];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_7[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_7[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 16 * 5; ch++) {
        float sc = stage2_block3_expand_bn_w[ch] / sqrtf(stage2_block3_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage2_block3_expand_bn_b[ch] - stage2_block3_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_7[ch * 1 * W + i] = texp_7[ch * 1 * W + i] * sc + sh;
    }
    float* tout_7 = &kws_scratch_buf[OFS_tout_7];
    for (oc = 0; oc < 16; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_7[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_7[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=2 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_7 = &kws_scratch_buf[OFS_br_p_7];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_7[oc * 5 * W + ih * W + ow];
            br_p_7[oc * W + ow] = s / 5;
        }
    float* br_d_7 = &kws_scratch_buf[OFS_br_d_7];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 2;
                if (iw >= 0 && iw < W) s += br_p_7[oc * W + iw] * stage2_block3_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage2_block3_br_bn_mean[oc]) / sqrtf(stage2_block3_br_bn_var[oc] + 1.0e-5f)) * stage2_block3_br_bn_w[oc] + stage2_block3_br_bn_b[oc];
            br_d_7[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_7 = &kws_scratch_buf[OFS_br_o_7];
    for (oc = 0; oc < 16; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 16; ic++) s += br_d_7[ic * W + ow] * stage2_block3_br_proj_w[oc * 16 + ic];
            br_o_7[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_7 = &kws_scratch_buf[OFS_nxt_7];
    for (oc = 0; oc < 16; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_7[oc * 5 * W + oh * W + ow] + br_o_7[oc * W + ow];
                if (oc < 16) {
                    v += cur[oc * 5 * W + oh * W + ow];
                }
                nxt_7[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_7;

    /* --- stage3_block0: 16->20, H=5->5, dil=4, exp=5 --- */
    float* t1_8 = &kws_scratch_buf[OFS_t1_8];
    /* 1x1 Conv */
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 16; ic++)
                    s += cur[ic * 5 * W + oh * W + ow] * stage3_block0_main_conv1x1_w[oc * 16 + ic];
                s = ((s - stage3_block0_main_bn1_mean[oc]) / sqrtf(stage3_block0_main_bn1_var[oc] + 1.0e-5f)) * stage3_block0_main_bn1_w[oc] + stage3_block0_main_bn1_b[oc];
                t1_8[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_8 = &kws_scratch_buf[OFS_t2_8];
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 5)
                        s += t1_8[oc * 5 * W + ih * W + ow] * stage3_block0_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage3_block0_main_bn2_mean[oc]) / sqrtf(stage3_block0_main_bn2_var[oc] + 1.0e-5f)) * stage3_block0_main_bn2_w[oc] + stage3_block0_main_bn2_b[oc];
                t2_8[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (20,5,W) -> (20*5,1,W) -> BN -> back */
    float* texp_8 = &kws_scratch_buf[OFS_texp_8];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_8[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_8[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 20 * 5; ch++) {
        float sc = stage3_block0_expand_bn_w[ch] / sqrtf(stage3_block0_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage3_block0_expand_bn_b[ch] - stage3_block0_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_8[ch * 1 * W + i] = texp_8[ch * 1 * W + i] * sc + sh;
    }
    float* tout_8 = &kws_scratch_buf[OFS_tout_8];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_8[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_8[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=4 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_8 = &kws_scratch_buf[OFS_br_p_8];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_8[oc * 5 * W + ih * W + ow];
            br_p_8[oc * W + ow] = s / 5;
        }
    float* br_d_8 = &kws_scratch_buf[OFS_br_d_8];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 4;
                if (iw >= 0 && iw < W) s += br_p_8[oc * W + iw] * stage3_block0_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage3_block0_br_bn_mean[oc]) / sqrtf(stage3_block0_br_bn_var[oc] + 1.0e-5f)) * stage3_block0_br_bn_w[oc] + stage3_block0_br_bn_b[oc];
            br_d_8[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_8 = &kws_scratch_buf[OFS_br_o_8];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 20; ic++) s += br_d_8[ic * W + ow] * stage3_block0_br_proj_w[oc * 20 + ic];
            br_o_8[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_8 = &kws_scratch_buf[OFS_nxt_8];
    for (oc = 0; oc < 20; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_8[oc * 5 * W + oh * W + ow] + br_o_8[oc * W + ow];
                nxt_8[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_8;

    /* --- stage3_block1: 20->20, H=5->5, dil=4, exp=5 --- */
    float* t1_9 = &kws_scratch_buf[OFS_t1_9];
    /* 1x1 Conv */
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 20; ic++)
                    s += cur[ic * 5 * W + oh * W + ow] * stage3_block1_main_conv1x1_w[oc * 20 + ic];
                s = ((s - stage3_block1_main_bn1_mean[oc]) / sqrtf(stage3_block1_main_bn1_var[oc] + 1.0e-5f)) * stage3_block1_main_bn1_w[oc] + stage3_block1_main_bn1_b[oc];
                t1_9[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_9 = &kws_scratch_buf[OFS_t2_9];
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 5)
                        s += t1_9[oc * 5 * W + ih * W + ow] * stage3_block1_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage3_block1_main_bn2_mean[oc]) / sqrtf(stage3_block1_main_bn2_var[oc] + 1.0e-5f)) * stage3_block1_main_bn2_w[oc] + stage3_block1_main_bn2_b[oc];
                t2_9[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (20,5,W) -> (20*5,1,W) -> BN -> back */
    float* texp_9 = &kws_scratch_buf[OFS_texp_9];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_9[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_9[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 20 * 5; ch++) {
        float sc = stage3_block1_expand_bn_w[ch] / sqrtf(stage3_block1_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage3_block1_expand_bn_b[ch] - stage3_block1_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_9[ch * 1 * W + i] = texp_9[ch * 1 * W + i] * sc + sh;
    }
    float* tout_9 = &kws_scratch_buf[OFS_tout_9];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_9[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_9[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=4 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_9 = &kws_scratch_buf[OFS_br_p_9];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_9[oc * 5 * W + ih * W + ow];
            br_p_9[oc * W + ow] = s / 5;
        }
    float* br_d_9 = &kws_scratch_buf[OFS_br_d_9];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 4;
                if (iw >= 0 && iw < W) s += br_p_9[oc * W + iw] * stage3_block1_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage3_block1_br_bn_mean[oc]) / sqrtf(stage3_block1_br_bn_var[oc] + 1.0e-5f)) * stage3_block1_br_bn_w[oc] + stage3_block1_br_bn_b[oc];
            br_d_9[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_9 = &kws_scratch_buf[OFS_br_o_9];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 20; ic++) s += br_d_9[ic * W + ow] * stage3_block1_br_proj_w[oc * 20 + ic];
            br_o_9[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_9 = &kws_scratch_buf[OFS_nxt_9];
    for (oc = 0; oc < 20; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_9[oc * 5 * W + oh * W + ow] + br_o_9[oc * W + ow];
                if (oc < 20) {
                    v += cur[oc * 5 * W + oh * W + ow];
                }
                nxt_9[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_9;

    /* --- stage3_block2: 20->20, H=5->5, dil=4, exp=5 --- */
    float* t1_10 = &kws_scratch_buf[OFS_t1_10];
    /* 1x1 Conv */
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 20; ic++)
                    s += cur[ic * 5 * W + oh * W + ow] * stage3_block2_main_conv1x1_w[oc * 20 + ic];
                s = ((s - stage3_block2_main_bn1_mean[oc]) / sqrtf(stage3_block2_main_bn1_var[oc] + 1.0e-5f)) * stage3_block2_main_bn1_w[oc] + stage3_block2_main_bn1_b[oc];
                t1_10[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_10 = &kws_scratch_buf[OFS_t2_10];
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 5)
                        s += t1_10[oc * 5 * W + ih * W + ow] * stage3_block2_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage3_block2_main_bn2_mean[oc]) / sqrtf(stage3_block2_main_bn2_var[oc] + 1.0e-5f)) * stage3_block2_main_bn2_w[oc] + stage3_block2_main_bn2_b[oc];
                t2_10[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (20,5,W) -> (20*5,1,W) -> BN -> back */
    float* texp_10 = &kws_scratch_buf[OFS_texp_10];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_10[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_10[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 20 * 5; ch++) {
        float sc = stage3_block2_expand_bn_w[ch] / sqrtf(stage3_block2_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage3_block2_expand_bn_b[ch] - stage3_block2_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_10[ch * 1 * W + i] = texp_10[ch * 1 * W + i] * sc + sh;
    }
    float* tout_10 = &kws_scratch_buf[OFS_tout_10];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_10[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_10[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=4 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_10 = &kws_scratch_buf[OFS_br_p_10];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_10[oc * 5 * W + ih * W + ow];
            br_p_10[oc * W + ow] = s / 5;
        }
    float* br_d_10 = &kws_scratch_buf[OFS_br_d_10];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 4;
                if (iw >= 0 && iw < W) s += br_p_10[oc * W + iw] * stage3_block2_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage3_block2_br_bn_mean[oc]) / sqrtf(stage3_block2_br_bn_var[oc] + 1.0e-5f)) * stage3_block2_br_bn_w[oc] + stage3_block2_br_bn_b[oc];
            br_d_10[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_10 = &kws_scratch_buf[OFS_br_o_10];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 20; ic++) s += br_d_10[ic * W + ow] * stage3_block2_br_proj_w[oc * 20 + ic];
            br_o_10[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_10 = &kws_scratch_buf[OFS_nxt_10];
    for (oc = 0; oc < 20; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_10[oc * 5 * W + oh * W + ow] + br_o_10[oc * W + ow];
                if (oc < 20) {
                    v += cur[oc * 5 * W + oh * W + ow];
                }
                nxt_10[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_10;

    /* --- stage3_block3: 20->20, H=5->5, dil=4, exp=5 --- */
    float* t1_11 = &kws_scratch_buf[OFS_t1_11];
    /* 1x1 Conv */
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (ic = 0; ic < 20; ic++)
                    s += cur[ic * 5 * W + oh * W + ow] * stage3_block3_main_conv1x1_w[oc * 20 + ic];
                s = ((s - stage3_block3_main_bn1_mean[oc]) / sqrtf(stage3_block3_main_bn1_var[oc] + 1.0e-5f)) * stage3_block3_main_bn1_w[oc] + stage3_block3_main_bn1_b[oc];
                t1_11[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* 3x1 DWConv stride=1 */
    float* t2_11 = &kws_scratch_buf[OFS_t2_11];
    for (oc = 0; oc < 20; oc++) {
        for (oh = 0; oh < 5; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 3; fh++) {
                    long ih = oh * 1 + fh - 1;
                    if (ih >= 0 && ih < 5)
                        s += t1_11[oc * 5 * W + ih * W + ow] * stage3_block3_main_dwconv_w[oc * 3 + fh];
                }
                s = ((s - stage3_block3_main_bn2_mean[oc]) / sqrtf(stage3_block3_main_bn2_var[oc] + 1.0e-5f)) * stage3_block3_main_bn2_w[oc] + stage3_block3_main_bn2_b[oc];
                t2_11[oc * 5 * W + oh * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* Internal expand: (20,5,W) -> (20*5,1,W) -> BN -> back */
    float* texp_11 = &kws_scratch_buf[OFS_texp_11];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++)
                    texp_11[(oc * 5 + e) * 1 * W + oh * W + ow] = t2_11[oc * 5 * W + (oh * 5 + e) * W + ow];
    for (ch = 0; ch < 20 * 5; ch++) {
        float sc = stage3_block3_expand_bn_w[ch] / sqrtf(stage3_block3_expand_bn_var[ch] + 1.0e-5f);
        float sh = stage3_block3_expand_bn_b[ch] - stage3_block3_expand_bn_mean[ch] * sc;
        for (i = 0; i < 1 * W; i++)
            texp_11[ch * 1 * W + i] = texp_11[ch * 1 * W + i] * sc + sh;
    }
    float* tout_11 = &kws_scratch_buf[OFS_tout_11];
    for (oc = 0; oc < 20; oc++)
        for (e = 0; e < 5; e++)
            for (oh = 0; oh < 1; oh++)
                for (ow = 0; ow < W; ow++) {
                    float v = texp_11[(oc * 5 + e) * 1 * W + oh * W + ow];
                    tout_11[oc * 5 * W + (oh * 5 + e) * W + ow] = (v > 0) ? v : 0;
                }
    /* Broadcast residual: AvgPool -> 1x3 DWConv dil=4 -> BN -> Sigmoid -> 1x1 proj */
    float* br_p_11 = &kws_scratch_buf[OFS_br_p_11];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ih = 0; ih < 5; ih++) s += tout_11[oc * 5 * W + ih * W + ow];
            br_p_11[oc * W + ow] = s / 5;
        }
    float* br_d_11 = &kws_scratch_buf[OFS_br_d_11];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * 4;
                if (iw >= 0 && iw < W) s += br_p_11[oc * W + iw] * stage3_block3_br_dwconv_w[oc * 3 + fw];
            }
            s = ((s - stage3_block3_br_bn_mean[oc]) / sqrtf(stage3_block3_br_bn_var[oc] + 1.0e-5f)) * stage3_block3_br_bn_w[oc] + stage3_block3_br_bn_b[oc];
            br_d_11[oc * W + ow] = 1.0f / (1.0f + expf(-s));
        }
    float* br_o_11 = &kws_scratch_buf[OFS_br_o_11];
    for (oc = 0; oc < 20; oc++)
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < 20; ic++) s += br_d_11[ic * W + ow] * stage3_block3_br_proj_w[oc * 20 + ic];
            br_o_11[oc * W + ow] = s;
        }
    /* Add + ReLU */
    float* nxt_11 = &kws_scratch_buf[OFS_nxt_11];
    for (oc = 0; oc < 20; oc++)
        for (oh = 0; oh < 5; oh++)
            for (ow = 0; ow < W; ow++) {
                float v = tout_11[oc * 5 * W + oh * W + ow] + br_o_11[oc * W + ow];
                if (oc < 20) {
                    v += cur[oc * 5 * W + oh * W + ow];
                }
                nxt_11[oc * 5 * W + oh * W + ow] = (v > 0) ? v : 0;
            }
    cur = nxt_11;

    /* --- Final DWConv 5x5: (20,5,W) -> (20,1,W) --- */
    float* fd_out = &kws_scratch_buf[OFS_fd_out];
    for (oc = 0; oc < CHANNEL4; oc++) {
        for (oh = 0; oh < 1; oh++) {
            for (ow = 0; ow < W; ow++) {
                float s = 0;
                for (fh = 0; fh < 5; fh++) {
                    for (fw = 0; fw < 5; fw++) {
                        long ih = oh * 5 + fh - 2;
                        long iw = ow + fw - 2;
                        if (ih >= 0 && ih < 5 && iw >= 0 && iw < W)
                            s += cur[oc * 5 * W + ih * W + iw] * final_dwconv_w[(oc * 5 + fh) * 5 + fw];
                    }
                }
                s = ((s - final_dwconv_bn_mean[oc]) / sqrtf(final_dwconv_bn_var[oc] + 1.0e-5f)) * final_dwconv_bn_w[oc] + final_dwconv_bn_b[oc];
                fd_out[oc * W + ow] = (s > 0) ? s : 0;
            }
        }
    }
    /* --- Expand Conv 20->32 --- */
    float* ex_out = &kws_scratch_buf[OFS_ex_out];
    for (oc = 0; oc < CHANNEL5; oc++) {
        for (ow = 0; ow < W; ow++) {
            float s = 0;
            for (ic = 0; ic < CHANNEL4; ic++)
                s += fd_out[ic * W + ow] * expand_conv_w[oc * CHANNEL4 + ic];
            s = ((s - expand_bn_mean[oc]) / sqrtf(expand_bn_var[oc] + 1.0e-5f)) * expand_bn_w[oc] + expand_bn_b[oc];
            ex_out[oc * W + ow] = (s > 0) ? s : 0;
        }
    }
    /* --- Global AvgPool + Classifier --- */
    float logits[CLASSES_NUM];
    for (oc = 0; oc < CLASSES_NUM; oc++) {
        float s = classifier_b[oc];
        for (ic = 0; ic < CHANNEL5; ic++) {
            float avg = 0;
            for (ow = 0; ow < W; ow++) avg += ex_out[ic * W + ow];
            avg /= W;
            s += avg * classifier_w[oc * CHANNEL5 + ic];
        }
        logits[oc] = s;
    }
    /* --- Softmax + Argmax --- */
    {
        float max_logit = logits[0];
        float sum_exp = 0;
        long best = 0;
        for (i = 1; i < CLASSES_NUM; i++)
            if (logits[i] > logits[best]) best = i;
        max_logit = logits[best];
        for (i = 0; i < CLASSES_NUM; i++) {
            g_last_logits[i] = expf(logits[i] - max_logit);
            sum_exp += g_last_logits[i];
        }
        for (i = 0; i < CLASSES_NUM; i++)
            g_last_logits[i] /= (sum_exp + 1.0e-12f);
        g_last_conf = g_last_logits[best];
        return (int)best;
    }
}

static const char* g_names[CLASSES_NUM] = {
    "_silence_", "_unknown_", "down", "go", "left",
    "no", "off", "on", "right", "stop", "up", "yes"
};

const char* kws_label_name(int idx) {
    if (idx < 0 || idx >= CLASSES_NUM) return "???";
    return g_names[idx];
}

int kws_recognize_with_conf(const short waveform[], int signal_length, float* conf) {
    int result = kws_recognize(waveform, signal_length);
    if (conf) *conf = g_last_conf;
    return result;
}

int kws_recognize_with_logits(const short waveform[], int signal_length, float logits_out[CLASSES_NUM]) {
    int result = kws_recognize(waveform, signal_length);
    if (logits_out) {
        int i;
        for (i = 0; i < CLASSES_NUM; i++)
            logits_out[i] = g_last_logits[i];
    }
    return result;
}
