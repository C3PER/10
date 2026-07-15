/* BC-ResNet 权重 — 由 export_weights.py 自动生成 */
#ifndef KWS_WEIGHTS_H
#define KWS_WEIGHTS_H

#include <stddef.h>

/* ===== 特征提取参数 ===== */
#define FRM_LEN       160
#define WIN_SIZE      480
#define FFT_LEN       512
#define MELS_NUM      40
#define SAMPLES_NUM   16000
#define FREQ_NUM      257
#define CLASSES_NUM   12
#define FS            16000
#define PI            3.141592653589793

/* ===== BC-ResNet 网络结构 ===== */
#define CHANNEL0      16
#define CHANNEL1      8
#define CHANNEL2      12
#define CHANNEL3      16
#define CHANNEL4      20
#define CHANNEL5      32
#define STAGE_NUM     4

#define NUM_WEIGHT_ARRAYS  200  /* 实际数量见下方各组定义 */

/* 共 257 个权重数组 */

/* ===== Head Conv2d (1→16, 5×5, stride_h=2) ===== */
extern const float head_conv_w[400];  /* ((16, 1, 5, 5)) OIHW */
extern const float head_bn_w[16];  /* BN weight */
extern const float head_bn_b[16];  /* BN bias */
extern const float head_bn_mean[16];  /* BN mean */
extern const float head_bn_var[16];  /* BN var */

/* Stage 0 Block 0: 16→8 */
extern const float stage0_block0_main_conv1x1_w[128];  /* ((8, 16, 1, 1)) */
extern const float stage0_block0_main_bn1_w[8];  /* BN w */
extern const float stage0_block0_main_bn1_b[8];  /* BN b */
extern const float stage0_block0_main_bn1_mean[8];  /* BN mean */
extern const float stage0_block0_main_bn1_var[8];  /* BN var */
extern const float stage0_block0_main_dwconv_w[24];  /* ((8, 1, 3, 1)) */
extern const float stage0_block0_main_bn2_w[8];  /* BN w */
extern const float stage0_block0_main_bn2_b[8];  /* BN b */
extern const float stage0_block0_main_bn2_mean[8];  /* BN mean */
extern const float stage0_block0_main_bn2_var[8];  /* BN var */
extern const float stage0_block0_expand_bn_w[40];  /* ((40,)) */
extern const float stage0_block0_expand_bn_b[40];  /* BN b */
extern const float stage0_block0_expand_bn_mean[40];  /* BN mean */
extern const float stage0_block0_expand_bn_var[40];  /* BN var */
extern const float stage0_block0_br_dwconv_w[24];  /* ((8, 1, 1, 3)) */
extern const float stage0_block0_br_bn_w[8];  /* BN w */
extern const float stage0_block0_br_bn_b[8];  /* BN b */
extern const float stage0_block0_br_bn_mean[8];  /* BN mean */
extern const float stage0_block0_br_bn_var[8];  /* BN var */
extern const float stage0_block0_br_proj_w[64];  /* ((8, 8, 1, 1)) */

/* Stage 0 Block 1: 8→8 skip */
extern const float stage0_block1_main_conv1x1_w[64];  /* ((8, 8, 1, 1)) */
extern const float stage0_block1_main_bn1_w[8];  /* BN w */
extern const float stage0_block1_main_bn1_b[8];  /* BN b */
extern const float stage0_block1_main_bn1_mean[8];  /* BN mean */
extern const float stage0_block1_main_bn1_var[8];  /* BN var */
extern const float stage0_block1_main_dwconv_w[24];  /* ((8, 1, 3, 1)) */
extern const float stage0_block1_main_bn2_w[8];  /* BN w */
extern const float stage0_block1_main_bn2_b[8];  /* BN b */
extern const float stage0_block1_main_bn2_mean[8];  /* BN mean */
extern const float stage0_block1_main_bn2_var[8];  /* BN var */
extern const float stage0_block1_expand_bn_w[40];  /* ((40,)) */
extern const float stage0_block1_expand_bn_b[40];  /* BN b */
extern const float stage0_block1_expand_bn_mean[40];  /* BN mean */
extern const float stage0_block1_expand_bn_var[40];  /* BN var */
extern const float stage0_block1_br_dwconv_w[24];  /* ((8, 1, 1, 3)) */
extern const float stage0_block1_br_bn_w[8];  /* BN w */
extern const float stage0_block1_br_bn_b[8];  /* BN b */
extern const float stage0_block1_br_bn_mean[8];  /* BN mean */
extern const float stage0_block1_br_bn_var[8];  /* BN var */
extern const float stage0_block1_br_proj_w[64];  /* ((8, 8, 1, 1)) */

/* Stage 1 Block 0: 8→12 stride=2 */
extern const float stage1_block0_main_conv1x1_w[96];  /* ((12, 8, 1, 1)) */
extern const float stage1_block0_main_bn1_w[12];  /* BN w */
extern const float stage1_block0_main_bn1_b[12];  /* BN b */
extern const float stage1_block0_main_bn1_mean[12];  /* BN mean */
extern const float stage1_block0_main_bn1_var[12];  /* BN var */
extern const float stage1_block0_main_dwconv_w[36];  /* ((12, 1, 3, 1)) */
extern const float stage1_block0_main_bn2_w[12];  /* BN w */
extern const float stage1_block0_main_bn2_b[12];  /* BN b */
extern const float stage1_block0_main_bn2_mean[12];  /* BN mean */
extern const float stage1_block0_main_bn2_var[12];  /* BN var */
extern const float stage1_block0_expand_bn_w[60];  /* ((60,)) */
extern const float stage1_block0_expand_bn_b[60];  /* BN b */
extern const float stage1_block0_expand_bn_mean[60];  /* BN mean */
extern const float stage1_block0_expand_bn_var[60];  /* BN var */
extern const float stage1_block0_br_dwconv_w[36];  /* ((12, 1, 1, 3)) */
extern const float stage1_block0_br_bn_w[12];  /* BN w */
extern const float stage1_block0_br_bn_b[12];  /* BN b */
extern const float stage1_block0_br_bn_mean[12];  /* BN mean */
extern const float stage1_block0_br_bn_var[12];  /* BN var */
extern const float stage1_block0_br_proj_w[144];  /* ((12, 12, 1, 1)) */

/* Stage 1 Block 1: 12→12 skip */
extern const float stage1_block1_main_conv1x1_w[144];  /* ((12, 12, 1, 1)) */
extern const float stage1_block1_main_bn1_w[12];  /* BN w */
extern const float stage1_block1_main_bn1_b[12];  /* BN b */
extern const float stage1_block1_main_bn1_mean[12];  /* BN mean */
extern const float stage1_block1_main_bn1_var[12];  /* BN var */
extern const float stage1_block1_main_dwconv_w[36];  /* ((12, 1, 3, 1)) */
extern const float stage1_block1_main_bn2_w[12];  /* BN w */
extern const float stage1_block1_main_bn2_b[12];  /* BN b */
extern const float stage1_block1_main_bn2_mean[12];  /* BN mean */
extern const float stage1_block1_main_bn2_var[12];  /* BN var */
extern const float stage1_block1_expand_bn_w[60];  /* ((60,)) */
extern const float stage1_block1_expand_bn_b[60];  /* BN b */
extern const float stage1_block1_expand_bn_mean[60];  /* BN mean */
extern const float stage1_block1_expand_bn_var[60];  /* BN var */
extern const float stage1_block1_br_dwconv_w[36];  /* ((12, 1, 1, 3)) */
extern const float stage1_block1_br_bn_w[12];  /* BN w */
extern const float stage1_block1_br_bn_b[12];  /* BN b */
extern const float stage1_block1_br_bn_mean[12];  /* BN mean */
extern const float stage1_block1_br_bn_var[12];  /* BN var */
extern const float stage1_block1_br_proj_w[144];  /* ((12, 12, 1, 1)) */

/* Stage 2 Block 0: 12→16 stride=2 */
extern const float stage2_block0_main_conv1x1_w[192];  /* ((16, 12, 1, 1)) */
extern const float stage2_block0_main_bn1_w[16];  /* BN w */
extern const float stage2_block0_main_bn1_b[16];  /* BN b */
extern const float stage2_block0_main_bn1_mean[16];  /* BN mean */
extern const float stage2_block0_main_bn1_var[16];  /* BN var */
extern const float stage2_block0_main_dwconv_w[48];  /* ((16, 1, 3, 1)) */
extern const float stage2_block0_main_bn2_w[16];  /* BN w */
extern const float stage2_block0_main_bn2_b[16];  /* BN b */
extern const float stage2_block0_main_bn2_mean[16];  /* BN mean */
extern const float stage2_block0_main_bn2_var[16];  /* BN var */
extern const float stage2_block0_expand_bn_w[80];  /* ((80,)) */
extern const float stage2_block0_expand_bn_b[80];  /* BN b */
extern const float stage2_block0_expand_bn_mean[80];  /* BN mean */
extern const float stage2_block0_expand_bn_var[80];  /* BN var */
extern const float stage2_block0_br_dwconv_w[48];  /* ((16, 1, 1, 3)) */
extern const float stage2_block0_br_bn_w[16];  /* BN w */
extern const float stage2_block0_br_bn_b[16];  /* BN b */
extern const float stage2_block0_br_bn_mean[16];  /* BN mean */
extern const float stage2_block0_br_bn_var[16];  /* BN var */
extern const float stage2_block0_br_proj_w[256];  /* ((16, 16, 1, 1)) */

/* Stage 2 Block 1: 16→16 skip */
extern const float stage2_block1_main_conv1x1_w[256];  /* ((16, 16, 1, 1)) */
extern const float stage2_block1_main_bn1_w[16];  /* BN w */
extern const float stage2_block1_main_bn1_b[16];  /* BN b */
extern const float stage2_block1_main_bn1_mean[16];  /* BN mean */
extern const float stage2_block1_main_bn1_var[16];  /* BN var */
extern const float stage2_block1_main_dwconv_w[48];  /* ((16, 1, 3, 1)) */
extern const float stage2_block1_main_bn2_w[16];  /* BN w */
extern const float stage2_block1_main_bn2_b[16];  /* BN b */
extern const float stage2_block1_main_bn2_mean[16];  /* BN mean */
extern const float stage2_block1_main_bn2_var[16];  /* BN var */
extern const float stage2_block1_expand_bn_w[80];  /* ((80,)) */
extern const float stage2_block1_expand_bn_b[80];  /* BN b */
extern const float stage2_block1_expand_bn_mean[80];  /* BN mean */
extern const float stage2_block1_expand_bn_var[80];  /* BN var */
extern const float stage2_block1_br_dwconv_w[48];  /* ((16, 1, 1, 3)) */
extern const float stage2_block1_br_bn_w[16];  /* BN w */
extern const float stage2_block1_br_bn_b[16];  /* BN b */
extern const float stage2_block1_br_bn_mean[16];  /* BN mean */
extern const float stage2_block1_br_bn_var[16];  /* BN var */
extern const float stage2_block1_br_proj_w[256];  /* ((16, 16, 1, 1)) */

/* Stage 2 Block 2: 16→16 skip */
extern const float stage2_block2_main_conv1x1_w[256];  /* ((16, 16, 1, 1)) */
extern const float stage2_block2_main_bn1_w[16];  /* BN w */
extern const float stage2_block2_main_bn1_b[16];  /* BN b */
extern const float stage2_block2_main_bn1_mean[16];  /* BN mean */
extern const float stage2_block2_main_bn1_var[16];  /* BN var */
extern const float stage2_block2_main_dwconv_w[48];  /* ((16, 1, 3, 1)) */
extern const float stage2_block2_main_bn2_w[16];  /* BN w */
extern const float stage2_block2_main_bn2_b[16];  /* BN b */
extern const float stage2_block2_main_bn2_mean[16];  /* BN mean */
extern const float stage2_block2_main_bn2_var[16];  /* BN var */
extern const float stage2_block2_expand_bn_w[80];  /* ((80,)) */
extern const float stage2_block2_expand_bn_b[80];  /* BN b */
extern const float stage2_block2_expand_bn_mean[80];  /* BN mean */
extern const float stage2_block2_expand_bn_var[80];  /* BN var */
extern const float stage2_block2_br_dwconv_w[48];  /* ((16, 1, 1, 3)) */
extern const float stage2_block2_br_bn_w[16];  /* BN w */
extern const float stage2_block2_br_bn_b[16];  /* BN b */
extern const float stage2_block2_br_bn_mean[16];  /* BN mean */
extern const float stage2_block2_br_bn_var[16];  /* BN var */
extern const float stage2_block2_br_proj_w[256];  /* ((16, 16, 1, 1)) */

/* Stage 2 Block 3: 16→16 skip */
extern const float stage2_block3_main_conv1x1_w[256];  /* ((16, 16, 1, 1)) */
extern const float stage2_block3_main_bn1_w[16];  /* BN w */
extern const float stage2_block3_main_bn1_b[16];  /* BN b */
extern const float stage2_block3_main_bn1_mean[16];  /* BN mean */
extern const float stage2_block3_main_bn1_var[16];  /* BN var */
extern const float stage2_block3_main_dwconv_w[48];  /* ((16, 1, 3, 1)) */
extern const float stage2_block3_main_bn2_w[16];  /* BN w */
extern const float stage2_block3_main_bn2_b[16];  /* BN b */
extern const float stage2_block3_main_bn2_mean[16];  /* BN mean */
extern const float stage2_block3_main_bn2_var[16];  /* BN var */
extern const float stage2_block3_expand_bn_w[80];  /* ((80,)) */
extern const float stage2_block3_expand_bn_b[80];  /* BN b */
extern const float stage2_block3_expand_bn_mean[80];  /* BN mean */
extern const float stage2_block3_expand_bn_var[80];  /* BN var */
extern const float stage2_block3_br_dwconv_w[48];  /* ((16, 1, 1, 3)) */
extern const float stage2_block3_br_bn_w[16];  /* BN w */
extern const float stage2_block3_br_bn_b[16];  /* BN b */
extern const float stage2_block3_br_bn_mean[16];  /* BN mean */
extern const float stage2_block3_br_bn_var[16];  /* BN var */
extern const float stage2_block3_br_proj_w[256];  /* ((16, 16, 1, 1)) */

/* Stage 3 Block 0: 16→20 */
extern const float stage3_block0_main_conv1x1_w[320];  /* ((20, 16, 1, 1)) */
extern const float stage3_block0_main_bn1_w[20];  /* BN w */
extern const float stage3_block0_main_bn1_b[20];  /* BN b */
extern const float stage3_block0_main_bn1_mean[20];  /* BN mean */
extern const float stage3_block0_main_bn1_var[20];  /* BN var */
extern const float stage3_block0_main_dwconv_w[60];  /* ((20, 1, 3, 1)) */
extern const float stage3_block0_main_bn2_w[20];  /* BN w */
extern const float stage3_block0_main_bn2_b[20];  /* BN b */
extern const float stage3_block0_main_bn2_mean[20];  /* BN mean */
extern const float stage3_block0_main_bn2_var[20];  /* BN var */
extern const float stage3_block0_expand_bn_w[100];  /* ((100,)) */
extern const float stage3_block0_expand_bn_b[100];  /* BN b */
extern const float stage3_block0_expand_bn_mean[100];  /* BN mean */
extern const float stage3_block0_expand_bn_var[100];  /* BN var */
extern const float stage3_block0_br_dwconv_w[60];  /* ((20, 1, 1, 3)) */
extern const float stage3_block0_br_bn_w[20];  /* BN w */
extern const float stage3_block0_br_bn_b[20];  /* BN b */
extern const float stage3_block0_br_bn_mean[20];  /* BN mean */
extern const float stage3_block0_br_bn_var[20];  /* BN var */
extern const float stage3_block0_br_proj_w[400];  /* ((20, 20, 1, 1)) */

/* Stage 3 Block 1: 20→20 skip */
extern const float stage3_block1_main_conv1x1_w[400];  /* ((20, 20, 1, 1)) */
extern const float stage3_block1_main_bn1_w[20];  /* BN w */
extern const float stage3_block1_main_bn1_b[20];  /* BN b */
extern const float stage3_block1_main_bn1_mean[20];  /* BN mean */
extern const float stage3_block1_main_bn1_var[20];  /* BN var */
extern const float stage3_block1_main_dwconv_w[60];  /* ((20, 1, 3, 1)) */
extern const float stage3_block1_main_bn2_w[20];  /* BN w */
extern const float stage3_block1_main_bn2_b[20];  /* BN b */
extern const float stage3_block1_main_bn2_mean[20];  /* BN mean */
extern const float stage3_block1_main_bn2_var[20];  /* BN var */
extern const float stage3_block1_expand_bn_w[100];  /* ((100,)) */
extern const float stage3_block1_expand_bn_b[100];  /* BN b */
extern const float stage3_block1_expand_bn_mean[100];  /* BN mean */
extern const float stage3_block1_expand_bn_var[100];  /* BN var */
extern const float stage3_block1_br_dwconv_w[60];  /* ((20, 1, 1, 3)) */
extern const float stage3_block1_br_bn_w[20];  /* BN w */
extern const float stage3_block1_br_bn_b[20];  /* BN b */
extern const float stage3_block1_br_bn_mean[20];  /* BN mean */
extern const float stage3_block1_br_bn_var[20];  /* BN var */
extern const float stage3_block1_br_proj_w[400];  /* ((20, 20, 1, 1)) */

/* Stage 3 Block 2: 20→20 skip */
extern const float stage3_block2_main_conv1x1_w[400];  /* ((20, 20, 1, 1)) */
extern const float stage3_block2_main_bn1_w[20];  /* BN w */
extern const float stage3_block2_main_bn1_b[20];  /* BN b */
extern const float stage3_block2_main_bn1_mean[20];  /* BN mean */
extern const float stage3_block2_main_bn1_var[20];  /* BN var */
extern const float stage3_block2_main_dwconv_w[60];  /* ((20, 1, 3, 1)) */
extern const float stage3_block2_main_bn2_w[20];  /* BN w */
extern const float stage3_block2_main_bn2_b[20];  /* BN b */
extern const float stage3_block2_main_bn2_mean[20];  /* BN mean */
extern const float stage3_block2_main_bn2_var[20];  /* BN var */
extern const float stage3_block2_expand_bn_w[100];  /* ((100,)) */
extern const float stage3_block2_expand_bn_b[100];  /* BN b */
extern const float stage3_block2_expand_bn_mean[100];  /* BN mean */
extern const float stage3_block2_expand_bn_var[100];  /* BN var */
extern const float stage3_block2_br_dwconv_w[60];  /* ((20, 1, 1, 3)) */
extern const float stage3_block2_br_bn_w[20];  /* BN w */
extern const float stage3_block2_br_bn_b[20];  /* BN b */
extern const float stage3_block2_br_bn_mean[20];  /* BN mean */
extern const float stage3_block2_br_bn_var[20];  /* BN var */
extern const float stage3_block2_br_proj_w[400];  /* ((20, 20, 1, 1)) */

/* Stage 3 Block 3: 20→20 skip */
extern const float stage3_block3_main_conv1x1_w[400];  /* ((20, 20, 1, 1)) */
extern const float stage3_block3_main_bn1_w[20];  /* BN w */
extern const float stage3_block3_main_bn1_b[20];  /* BN b */
extern const float stage3_block3_main_bn1_mean[20];  /* BN mean */
extern const float stage3_block3_main_bn1_var[20];  /* BN var */
extern const float stage3_block3_main_dwconv_w[60];  /* ((20, 1, 3, 1)) */
extern const float stage3_block3_main_bn2_w[20];  /* BN w */
extern const float stage3_block3_main_bn2_b[20];  /* BN b */
extern const float stage3_block3_main_bn2_mean[20];  /* BN mean */
extern const float stage3_block3_main_bn2_var[20];  /* BN var */
extern const float stage3_block3_expand_bn_w[100];  /* ((100,)) */
extern const float stage3_block3_expand_bn_b[100];  /* BN b */
extern const float stage3_block3_expand_bn_mean[100];  /* BN mean */
extern const float stage3_block3_expand_bn_var[100];  /* BN var */
extern const float stage3_block3_br_dwconv_w[60];  /* ((20, 1, 1, 3)) */
extern const float stage3_block3_br_bn_w[20];  /* BN w */
extern const float stage3_block3_br_bn_b[20];  /* BN b */
extern const float stage3_block3_br_bn_mean[20];  /* BN mean */
extern const float stage3_block3_br_bn_var[20];  /* BN var */
extern const float stage3_block3_br_proj_w[400];  /* ((20, 20, 1, 1)) */

/* ===== Final DWConv 5×5 ===== */
extern const float final_dwconv_w[500];  /* ((20, 1, 5, 5)) */
extern const float final_dwconv_bn_w[20];  /* BN w */
extern const float final_dwconv_bn_b[20];  /* BN b */
extern const float final_dwconv_bn_mean[20];  /* BN mean */
extern const float final_dwconv_bn_var[20];  /* BN var */

/* ===== Expand Conv 1×1 ===== */
extern const float expand_conv_w[640];  /* ((32, 20, 1, 1)) */
extern const float expand_bn_w[32];  /* BN w */
extern const float expand_bn_b[32];  /* BN b */
extern const float expand_bn_mean[32];  /* BN mean */
extern const float expand_bn_var[32];  /* BN var */

/* ===== Classifier Conv 1×1 ===== */
extern const float classifier_w[384];  /* ((12, 32, 1, 1)) */
extern const float classifier_b[12];  /* bias */

#endif /* KWS_WEIGHTS_H */
