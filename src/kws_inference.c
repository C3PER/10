/* BC-ResNet C 推理 — 特征提取 + 网络前向推理 */
#include "kws_inference.h"
#include "kws_weights.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* ============================================================
   全局预计算 buffer
   ============================================================ */
static double g_window[WIN_SIZE];                         /* Hann 窗 */
static double g_mel_fb[FREQ_NUM * MELS_NUM];             /* Mel 滤波器组 */

/* 运行时中间 buffer */
static double g_fft_re[FFT_LEN];
static double g_fft_im[FFT_LEN];

/* ============================================================
   工具函数
   ============================================================ */

static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }

/* 简单版 FFT (与指导书 C 代码一致，Cooley-Tukey 基-2) */
static void fft(double* x, double* y, long n, long sign) {
    long i, j, k, m, n1, n2;
    double c, c1, e, s, s1, t, tr, ti;
    for (j = i = 1; i < 31; i++) {
        m = i;
        j = 2 * j;
        if (j == n) break;
    }
    for (n1 = n - 1, j = 0, i = 0; i < n1; i++) {
        if (i < j) {
            tr = x[j]; ti = y[j];
            x[j] = x[i]; y[j] = y[i];
            x[i] = tr; y[i] = ti;
        }
        k = n / 2;
        while (k < (j + 1)) { j = j - k; k = k / 2; }
        j = j + k;
    }
    e = 0; c = 1.0; s = 0.0;
    for (n2 = 1; n2 < n; n2 *= 2) {
        c1 = cos(PI / n2);
        s1 = -sign * sin(PI / n2);
        n1 = 2 * n2;
        for (j = 0; j < n2; j++) {
            for (i = j; i < n; i += n1) {
                k = i + n2;
                tr = c * x[k] - s * y[k];
                ti = c * y[k] + s * x[k];
                x[k] = x[i] - tr;
                y[k] = y[i] - ti;
                x[i] = x[i] + tr;
                y[i] = y[i] + ti;
            }
            t = c;
            c = c * c1 - s * s1;
            s = t * s1 + s * c1;
        }
    }
    if (sign == -1) {
        for (i = 0; i < n; i++) { x[i] /= n; y[i] /= n; }
    }
}

/* ============================================================
   初始化
   ============================================================ */
void kws_init(void) {
    long i, j;

    /* Hann 窗 */
    for (i = 0; i < WIN_SIZE; i++)
        g_window[i] = 0.5 * (1.0 - cos(2.0 * PI * i / WIN_SIZE));

    /* Mel 滤波器组 */
    double all_freqs[FREQ_NUM];
    double m_pts[MELS_NUM + 2], f_pts[MELS_NUM + 2], f_diff[MELS_NUM + 1];
    double f_min = 0.0, f_max = FS / 2.0;

    for (i = 0; i < FREQ_NUM; i++)
        all_freqs[i] = (double)i * f_max / (FREQ_NUM - 1);

    double m_min = 2595.0 * log10(1.0 + f_min / 700.0);
    double m_max = 2595.0 * log10(1.0 + f_max / 700.0);

    for (i = 0; i <= MELS_NUM; i++) {
        m_pts[i + 1] = m_min + (m_max - m_min) * (i + 1) / (MELS_NUM + 1);
        f_pts[i + 1] = 700.0 * (pow(10.0, m_pts[i + 1] / 2595.0) - 1.0);
    }
    m_pts[0] = 0; f_pts[0] = 0;
    for (i = 0; i < MELS_NUM + 1; i++)
        f_diff[i] = f_pts[i + 1] - f_pts[i];

    for (i = 0; i < FREQ_NUM; i++) {
        for (j = 0; j < MELS_NUM; j++) {
            double low  = -(all_freqs[i] - f_pts[j])     / (f_diff[j] + 1e-12);
            double high =  (all_freqs[i] - f_pts[j + 2]) / (f_diff[j + 1] + 1e-12);
            g_mel_fb[i * MELS_NUM + j] = dmax(0.0, dmin(low, high));
        }
    }
}

/* ============================================================
   Log-Mel 特征提取
   ============================================================ */
static void extract_log_mel(const short waveform[], int signal_length,
                            double* mel_out) {
    long i, n, k, m;
    long num_frames = signal_length / FRM_LEN + 1;  /* ~101 */
    long pad_len = WIN_SIZE / 2;                     /* 240 */

    for (n = 0; n < num_frames; n++) {
        /* 设置 FFT 实部 (加窗) */
        for (i = 0; i < FFT_LEN; i++) {
            g_fft_re[i] = 0.0;
            g_fft_im[i] = 0.0;
        }
        for (i = 0; i < WIN_SIZE; i++) {
            long src_idx = n * FRM_LEN + i - pad_len;
            double val = 0.0;
            if (src_idx >= 0 && src_idx < signal_length)
                val = waveform[src_idx] / 32768.0;
            g_fft_re[i] = val * g_window[i];
        }

        fft(g_fft_re, g_fft_im, FFT_LEN, 1);

        /* 功率谱 → Mel 滤波器组 → log */
        for (m = 0; m < MELS_NUM; m++) {
            double accum = 0.0;
            for (k = 0; k < FREQ_NUM; k++) {
                double power = g_fft_re[k] * g_fft_re[k] + g_fft_im[k] * g_fft_im[k];
                accum += power * g_mel_fb[k * MELS_NUM + m];
            }
            mel_out[n * MELS_NUM + m] = log(accum + 1.0e-6);
        }
    }
}

/* ============================================================
   Conv2d + BN + ReLU (含 padding 和 stride)
   ============================================================ */
static void conv_bn_relu(
    const double* input,   /* [in_c][in_h][in_w] */
    long in_c, long in_h, long in_w,
    const double* weight,  /* [out_c][in_c][kh][kw] OIHW */
    const double* bn_w, const double* bn_b,
    const double* bn_mean, const double* bn_var,
    long out_c, long kh, long kw,
    long stride_h, long stride_w,
    long pad_h, long pad_w,
    double* output)        /* [out_c][out_h][out_w] */
{
    long out_h = (in_h + 2 * pad_h - kh) / stride_h + 1;
    long out_w = (in_w + 2 * pad_w - kw) / stride_w + 1;

    for (long oc = 0; oc < out_c; oc++) {
        for (long oh = 0; oh < out_h; oh++) {
            for (long ow = 0; ow < out_w; ow++) {
                double sum = 0.0;
                for (long ic = 0; ic < in_c; ic++) {
                    for (long fh = 0; fh < kh; fh++) {
                        for (long fw = 0; fw < kw; fw++) {
                            long ih = oh * stride_h + fh - pad_h;
                            long iw = ow * stride_w + fw - pad_w;
                            if (ih >= 0 && ih < in_h && iw >= 0 && iw < in_w) {
                                double v = input[ic * in_h * in_w + ih * in_w + iw];
                                /* weight[oc][ic][fh][fw] = weight[((oc * in_c + ic) * kh + fh) * kw + fw] */
                                double w = weight[((oc * in_c + ic) * kh + fh) * kw + fw];
                                sum += v * w;
                            }
                        }
                    }
                }
                /* BN + ReLU */
                double val = (sum - bn_mean[oc]) / sqrt(bn_var[oc] + 1.0e-5) * bn_w[oc] + bn_b[oc];
                output[oc * out_h * out_w + oh * out_w + ow] = (val > 0.0) ? val : 0.0;
            }
        }
    }
}

/* ============================================================
   Depthwise Conv2d (groups = out_c), no BN
   ============================================================ */
static void dwconv(
    const double* input,   /* [c][in_h][in_w] */
    long c, long in_h, long in_w,
    const double* weight,  /* [c][1][kh][kw] */
    long kh, long kw,
    long stride_h, long stride_w,
    long pad_h, long pad_w,
    long dilation_h, long dilation_w,
    double* output)        /* [c][out_h][out_w] */
{
    long out_h = (in_h + 2 * pad_h - dilation_h * (kh - 1) - 1) / stride_h + 1;
    long out_w = (in_w + 2 * pad_w - dilation_w * (kw - 1) - 1) / stride_w + 1;

    for (long oc = 0; oc < c; oc++) {
        for (long oh = 0; oh < out_h; oh++) {
            for (long ow = 0; ow < out_w; ow++) {
                double sum = 0.0;
                for (long fh = 0; fh < kh; fh++) {
                    for (long fw = 0; fw < kw; fw++) {
                        long ih = oh * stride_h + fh * dilation_h - pad_h;
                        long iw = ow * stride_w + fw * dilation_w - pad_w;
                        if (ih >= 0 && ih < in_h && iw >= 0 && iw < in_w) {
                            double v = input[oc * in_h * in_w + ih * in_w + iw];
                            double w = weight[((oc * 1 + 0) * kh + fh) * kw + fw];
                            sum += v * w;
                        }
                    }
                }
                output[oc * out_h * out_w + oh * out_w + ow] = sum;
            }
        }
    }
}

/* ============================================================
   1×1 Conv2d (no BN)
   ============================================================ */
static void conv1x1(
    const double* input,   /* [in_c][in_h][in_w] */
    long in_c, long in_h, long in_w,
    const double* weight,  /* [out_c][in_c][1][1] */
    long out_c,
    double* output)        /* [out_c][in_h][in_w] */
{
    for (long oc = 0; oc < out_c; oc++) {
        for (long oh = 0; oh < in_h; oh++) {
            for (long ow = 0; ow < in_w; ow++) {
                double sum = 0.0;
                for (long ic = 0; ic < in_c; ic++) {
                    double v = input[ic * in_h * in_w + oh * in_w + ow];
                    double w = weight[oc * in_c + ic];  /* 1×1 kernel */
                    sum += v * w;
                }
                output[oc * in_h * in_w + oh * in_w + ow] = sum;
            }
        }
    }
}

/* ============================================================
   BN + ReLU (in-place on feature map)
   ============================================================ */
static void bn_relu(
    double* data, long c, long h, long w,
    const double* bn_w, const double* bn_b,
    const double* bn_mean, const double* bn_var) {
    long spatial = h * w;
    for (long oc = 0; oc < c; oc++) {
        double scale = bn_w[oc] / sqrt(bn_var[oc] + 1.0e-5);
        double shift = bn_b[oc] - bn_mean[oc] * scale;
        for (long i = 0; i < spatial; i++) {
            double val = data[oc * spatial + i] * scale + shift;
            data[oc * spatial + i] = (val > 0.0) ? val : 0.0;
        }
    }
}

/* ============================================================
   AdaptiveAvgPool2d → (target_h, target_w)
   ============================================================ */
static void adaptive_avg_pool(
    const double* input, long c, long in_h, long in_w,
    long out_h, long out_w,
    double* output) {
    for (long oc = 0; oc < c; oc++) {
        for (long oh = 0; oh < out_h; oh++) {
            for (long ow = 0; ow < out_w; ow++) {
                /* 简化实现：目标尺寸小，直接算起止范围 */
                long h_start = (long)(oh * in_h / (double)out_h);
                long h_end   = (long)((oh + 1) * in_h / (double)out_h);
                long w_start = (long)(ow * in_w / (double)out_w);
                long w_end   = (long)((ow + 1) * in_w / (double)out_w);
                double sum = 0.0;
                long count = 0;
                for (long ih = h_start; ih < h_end; ih++) {
                    for (long iw = w_start; iw < w_end; iw++) {
                        sum += input[oc * in_h * in_w + ih * in_w + iw];
                        count++;
                    }
                }
                output[oc * out_h * out_w + oh * out_w + ow] = sum / (double)count;
            }
        }
    }
}

/* ============================================================
   Sigmoid
   ============================================================ */
static inline double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

/* ============================================================
   BC-ResBlock 核心
   ============================================================ */
static void bc_resblock_forward(
    double* x,              /* [in_c][H][W], in-place */
    long in_c, long out_c,
    long H, long W,
    long freq_stride,
    long dilation,
    long expansion,
    int use_skip,
    /* weights: need all block weights as parameters */
    const double* main_conv1x1_w,
    const double* main_bn1_w, const double* main_bn1_b,
    const double* main_bn1_mean, const double* main_bn1_var,
    const double* main_dwconv_w,
    const double* main_bn2_w, const double* main_bn2_b,
    const double* main_bn2_mean, const double* main_bn2_var,
    const double* expand_bn_w, const double* expand_bn_b,
    const double* expand_bn_mean, const double* expand_bn_var,
    const double* br_dwconv_w,
    const double* br_bn_w, const double* br_bn_b,
    const double* br_bn_mean, const double* br_bn_var,
    const double* br_proj_w)
{
    /* 1. 主路径 1×1 conv + BN + ReLU */
    long H1 = H;
    double* t1 = (double*)malloc(out_c * H1 * W * sizeof(double));
    conv_bn_relu(x, in_c, H, W, main_conv1x1_w,
                 main_bn1_w, main_bn1_b, main_bn1_mean, main_bn1_var,
                 out_c, 1, 1, 1, 1, 0, 0, t1);

    /* 2. 3×1 DWConv + BN + ReLU, stride=(freq_stride, 1) */
    long H2 = (H1 + 2 - 3) / freq_stride + 1;
    double* t2 = (double*)malloc(out_c * H2 * W * sizeof(double));

    long out_h, out_w;
    out_h = (H1 + 2 * 1 - 3) / freq_stride + 1;
    out_w = W;
    for (long oc = 0; oc < out_c; oc++) {
        for (long oh = 0; oh < out_h; oh++) {
            for (long ow = 0; ow < out_w; ow++) {
                double sum = 0.0;
                for (long fh = 0; fh < 3; fh++) {
                    long ih = oh * freq_stride + fh - 1;
                    if (ih >= 0 && ih < H1) {
                        double v = t1[oc * H1 * W + ih * W + ow];
                        double w = main_dwconv_w[((oc * 1 + 0) * 3 + fh) * 1 + 0];
                        sum += v * w;
                    }
                }
                sum = (sum - main_bn2_mean[oc]) / sqrt(main_bn2_var[oc] + 1.0e-5)
                    * main_bn2_w[oc] + main_bn2_b[oc];
                t2[oc * out_h * W + oh * W + ow] = (sum > 0.0) ? sum : 0.0;
            }
        }
    }

    /* 3. 内部扩展 reshape → BN → reshape back */
    long exp = expansion;
    long H2_div = H2 / exp;
    double* t2_expanded = (double*)malloc(out_c * exp * H2_div * W * sizeof(double));
    /* reshape: (C, H2, W) → (C, exp, H2/exp, W) → (C*exp, H2/exp, W) */
    for (long oc = 0; oc < out_c; oc++) {
        for (long e = 0; e < exp; e++) {
            for (long oh = 0; oh < H2_div; oh++) {
                for (long ow = 0; ow < W; ow++) {
                    double v = t2[oc * H2 * W + (oh * exp + e) * W + ow];
                    long dst_c = oc * exp + e;
                    t2_expanded[dst_c * H2_div * W + oh * W + ow] = v;
                }
            }
        }
    }
    /* BN on expanded */
    for (long ch = 0; ch < out_c * exp; ch++) {
        double scale = expand_bn_w[ch] / sqrt(expand_bn_var[ch] + 1.0e-5);
        double shift = expand_bn_b[ch] - expand_bn_mean[ch] * scale;
        for (long i = 0; i < H2_div * W; i++) {
            double val = t2_expanded[ch * H2_div * W + i] * scale + shift;
            t2_expanded[ch * H2_div * W + i] = val;
        }
    }
    /* reshape back: (C*exp, H2/exp, W) → (C, H2, W) */
    double* t2_restored = (double*)malloc(out_c * H2 * W * sizeof(double));
    for (long oc = 0; oc < out_c; oc++) {
        for (long e = 0; e < exp; e++) {
            for (long oh = 0; oh < H2_div; oh++) {
                for (long ow = 0; ow < W; ow++) {
                    long src_c = oc * exp + e;
                    double v = t2_expanded[src_c * H2_div * W + oh * W + ow];
                    t2_restored[oc * H2 * W + (oh * exp + e) * W + ow] = (v > 0.0) ? v : 0.0;
                }
            }
        }
    }
    free(t2_expanded);
    double* out_main = t2_restored;

    /* 4. 广播残差路径: AvgPool(H→1) → 1×3 DWConv(dilated) → BN → Sigmoid → 1×1 Conv */
    double* br_pooled = (double*)malloc(out_c * 1 * W * sizeof(double));
    for (long oc = 0; oc < out_c; oc++) {
        for (long ow = 0; ow < W; ow++) {
            double sum = 0.0;
            for (long ih = 0; ih < H2; ih++)
                sum += out_main[oc * H2 * W + ih * W + ow];
            br_pooled[oc * W + ow] = sum / (double)H2;
        }
    }

    double* br_dw = (double*)malloc(out_c * 1 * W * sizeof(double));
    for (long oc = 0; oc < out_c; oc++) {
        for (long ow = 0; ow < W; ow++) {
            double sum = 0.0;
            for (long fw = 0; fw < 3; fw++) {
                long iw = ow + (fw - 1) * dilation;
                if (iw >= 0 && iw < W) {
                    double v = br_pooled[oc * W + iw];
                    double w = br_dwconv_w[((oc * 1 + 0) * 1 + 0) * 3 + fw];
                    sum += v * w;
                }
            }
            sum = (sum - br_bn_mean[oc]) / sqrt(br_bn_var[oc] + 1.0e-5)
                * br_bn_w[oc] + br_bn_b[oc];
            br_dw[oc * W + ow] = sigmoid(sum);
        }
    }
    free(br_pooled);

    double* br_proj = (double*)malloc(out_c * 1 * W * sizeof(double));
    for (long oc = 0; oc < out_c; oc++) {
        for (long ow = 0; ow < W; ow++) {
            double sum = 0.0;
            for (long ic = 0; ic < out_c; ic++)
                sum += br_dw[ic * W + ow] * br_proj_w[oc * out_c + ic];
            br_proj[oc * W + ow] = sum;
        }
    }
    free(br_dw);

    /* 5. 加和 + ReLU */
    double* result = (double*)malloc(out_c * H2 * W * sizeof(double));
    for (long oc = 0; oc < out_c; oc++) {
        for (long oh = 0; oh < H2; oh++) {
            for (long ow = 0; ow < W; ow++) {
                double v = out_main[oc * H2 * W + oh * W + ow]
                         + br_proj[oc * W + ow];
                if (use_skip && oc < in_c) {
                    double skip;
                    if (in_c == out_c && H == H2) {
                        skip = x[oc * H * W + oh * W + ow];
                    } else {
                        /* adaptive avg pool for skip connection */
                        long h_start_skip = (long)(oh * H / (double)H2);
                        long h_end_skip   = (long)((oh + 1) * H / (double)H2);
                        double ssum = 0.0;
                        long scount = 0;
                        for (long ih = h_start_skip; ih < h_end_skip; ih++) {
                            ssum += x[oc * H * W + ih * W + ow];
                            scount++;
                        }
                        skip = ssum / (double)scount;
                    }
                    v += skip;
                }
                result[oc * H2 * W + oh * W + ow] = (v > 0.0) ? v : 0.0;
            }
        }
    }

    free(out_main);
    free(br_proj);

    /* Free old x and assign result */
    /* We can't free x here since it's passed in. The caller handles it. */
    /* Copy result back to x (requires x to already be sized for out_c × H2 × W) */
    /* We'll handle this in the caller by using a double pointer */
    memcpy(x, result, out_c * H2 * W * sizeof(double));
    free(result);
    free(t1);
    free(t2);
}

/* ============================================================
   主推理函数
   ============================================================ */
int kws_recognize(const short waveform[], int signal_length) {
    long W_frames = signal_length / FRM_LEN + 1;  /* ~101 */

    /* 1. 提取 Log-Mel 谱 */
    double* mel = (double*)malloc(W_frames * MELS_NUM * sizeof(double));
    extract_log_mel(waveform, signal_length, mel);

    /* 转换为 NCHW 格式: (1, MELS_NUM, W_frames) → 存储为 [c][h][w] */
    long H_mel = MELS_NUM;   /* 40 */
    long W_mel = W_frames;   /* 101 */
    double* feat = (double*)malloc(1 * H_mel * W_mel * sizeof(double));
    for (long h = 0; h < H_mel; h++)
        for (long w = 0; w < W_mel; w++)
            feat[h * W_mel + w] = mel[w * MELS_NUM + h];  /* transpose */
    free(mel);

    /* 2. Head Conv + BN + ReLU: 1×40×101 → 16×20×101 */
    long H0 = (H_mel + 2 * 2 - 5) / 2 + 1;  /* 20 */
    long W0 = W_mel;
    double* h0 = (double*)malloc(CHANNEL0 * H0 * W0 * sizeof(double));
    conv_bn_relu(feat, 1, H_mel, W_mel,
                 head_conv_w, head_bn_w, head_bn_b, head_bn_mean, head_bn_var,
                 CHANNEL0, 5, 5, 2, 1, 2, 2, h0);
    free(feat);

    /* 3. 逐 Stage 推理 */
    /* Stage configs: (in_ch, out_ch, num_blocks, dilation, first_freq_stride) */
    struct { long in_c, out_c, n, dil, stride; } stages[] = {
        {CHANNEL0, CHANNEL1, 2, 1, 1},   /* Stage 0: 16→8, H=20 */
        {CHANNEL1, CHANNEL2, 2, 1, 2},   /* Stage 1: 8→12,  H=10 */
        {CHANNEL2, CHANNEL3, 4, 2, 2},   /* Stage 2: 12→16, H=5 */
        {CHANNEL3, CHANNEL4, 4, 4, 1},   /* Stage 3: 16→20, H=5 */
    };
    long H_cur = H0;

    double* cur_feat = h0;
    for (long s = 0; s < 4; s++) {
        long in_c  = stages[s].in_c;
        long out_c = stages[s].out_c;
        long n_blk = stages[s].n;
        long dil   = stages[s].dil;

        for (long b = 0; b < n_blk; b++) {
            long b_in_c  = (b == 0) ? in_c : out_c;
            long b_stride = (b == 0) ? stages[s].stride : 1;

            /* 计算输出 H */
            long H_in  = H_cur;
            long H_out = (H_in + 2 * 1 - 3) / b_stride + 1;

            /* 分配新的 feature map buffer */
            double* new_feat = (double*)malloc(out_c * H_out * W_frames * sizeof(double));

            /* 先做 1×1 conv + BN + ReLU */
            double* t1 = (double*)malloc(out_c * H_in * W_frames * sizeof(double));
            /* Load block weights by name... We need weight accessor macros or a lookup */
            /* For now, directly use the exported symbol names */
            /* This is getting complex. Let me use a simpler approach. */
            free(t1);
            free(new_feat);

            /* TODO: Complete BC-ResBlock computation */
            H_cur = H_out;
        }
    }

    free(h0);
    return 0;
}

/* ============================================================
   标签名
   ============================================================ */
static const char* g_label_names[CLASSES_NUM] = {
    "_silence_", "_unknown_", "down", "go", "left",
    "no", "off", "on", "right", "stop", "up", "yes"
};

const char* kws_label_name(int idx) {
    if (idx < 0 || idx >= CLASSES_NUM) return "???";
    return g_label_names[idx];
}
