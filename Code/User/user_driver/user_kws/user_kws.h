/**
 * @file user_kws.h
 * @brief KWS 主循环 + LCD 显示
 * @details BC-ResNet 关键词识别，LCD 实时显示识别结果
 */

#ifndef _USER_KWS_H_
#define _USER_KWS_H_

#define KWS_WAVEFORM_SAMPLES  16000   /* 每次推理需要的采样点数 */
#define KWS_ADC_SAMPLES       8000    /* 单次 ADC 采集点数 (max 8192) */
#define KWS_ADC_FREQ          16000   /* ADC 采样率 (= 模型训练采样率) */

void Kws_Init(void);
void Kws_Main(void);

#endif /* _USER_KWS_H_ */
