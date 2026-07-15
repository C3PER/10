/* BC-ResNet C 推理 — 头文件 */
#ifndef KWS_INFERENCE_H
#define KWS_INFERENCE_H

#include <stddef.h>

/* ===== KWS 应用层配置 ===== */
#define KWS_ADC_FREQ            16000   /* ADC 采样率: 16kHz */
#define KWS_ADC_SAMPLES          8000   /* 单次采集点数 */
#define KWS_WAVEFORM_SAMPLES    16000   /* 总波形点数 (2 x 8000) */

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
