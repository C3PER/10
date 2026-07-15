/* BC-ResNet C 推理 — 头文件 */
#ifndef KWS_INFERENCE_H
#define KWS_INFERENCE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化：预计算 Hann 窗、Mel 滤波器组 */
void kws_init(void);

/* 推理：输入 16kHz/16bit PCM 波形 (16000 点)，返回类别索引 0-11 */
int kws_recognize(const short waveform[], int signal_length);

/* 获取各类别名称 */
const char* kws_label_name(int idx);

#ifdef __cplusplus
}
#endif

#endif /* KWS_INFERENCE_H */
