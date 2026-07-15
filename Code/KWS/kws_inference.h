/* BC-ResNet C 推理 — 头文件 */
#ifndef KWS_INFERENCE_H
#define KWS_INFERENCE_H

#include <stddef.h>
#include "kws_weights.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化：预计算 Hann 窗、Mel 滤波器组、FFT 旋转因子 */
void kws_init(void);

/* 推理：输入 16kHz/16bit PCM 波形 (16000 点)，返回类别索引 0-11 */
int kws_recognize(const short waveform[], int signal_length);

/* 推理并返回置信度：conf 输出最大 softmax 值 [0, 1] */
int kws_recognize_with_conf(const short waveform[], int signal_length, float* conf);

/* 推理并返回全部 12 类的 softmax 概率 (logits[12] 由调用者分配) */
int kws_recognize_with_logits(const short waveform[], int signal_length, float logits_out[CLASSES_NUM]);

/* 获取各类别名称 */
const char* kws_label_name(int idx);

#ifdef __cplusplus
}
#endif

#endif /* KWS_INFERENCE_H */
