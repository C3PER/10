"""
项目制实验3 — BC-ResNet 训练配置
"""
import torch
from pathlib import Path

# ============================================================
# 路径
# ============================================================
BASE_DIR = Path(__file__).resolve().parent.parent
DATA_DIR = BASE_DIR / "data"
CACHE_DIR = BASE_DIR / "data" / "processed"
OUTPUT_DIR = BASE_DIR / "outputs"
CHECKPOINT_DIR = OUTPUT_DIR / "checkpoints"
LOG_DIR = OUTPUT_DIR / "logs"

for d in [CHECKPOINT_DIR, LOG_DIR]:
    d.mkdir(parents=True, exist_ok=True)

# ============================================================
# 音频参数（与指导书 C 代码一致）
# ============================================================
SAMPLE_RATE = 16000
SIGNAL_LENGTH = 16000
FRAME_LENGTH = 160
WINDOW_SIZE = 480
FFT_SIZE = 512
MEL_BINS = 40
FREQ_NUM = FFT_SIZE // 2 + 1          # 257
NUM_FRAMES = SIGNAL_LENGTH // FRAME_LENGTH + 1  # 101

# ============================================================
# 类别
# ============================================================
TARGET_CLASSES = [
    '_silence_', '_unknown_', 'down', 'go', 'left',
    'no', 'off', 'on', 'right', 'stop', 'up', 'yes'
]
NUM_CLASSES = len(TARGET_CLASSES)     # 12
KEYWORDS = ['down', 'go', 'left', 'no', 'off', 'on', 'right', 'stop', 'up', 'yes']

# ============================================================
# BC-ResNet 网络结构
# ============================================================
# 各阶段输出通道（CHANNEL0～CHANNEL5）
CHANNELS = [16, 8, 12, 16, 20, 32]

# 各阶段子块配置: (输入通道索引, 输出通道索引, 子块数, 膨胀率, 首个子块频率stride)
STAGE_CONFIGS = [
    # (in_ch, out_ch, num_blocks, dilation, first_freq_stride)
    (16, 8,  2, 1, 1),   # Stage 0: 16→8
    (8,  12, 2, 1, 2),   # Stage 1: 8→12
    (12, 16, 4, 2, 2),   # Stage 2: 12→16
    (16, 20, 4, 4, 1),   # Stage 3: 16→20
]

HEAD_CONV_OUT = CHANNELS[0]           # 16
HEAD_KERNEL = 5
HEAD_STRIDE_H = 2

FINAL_DWCONV_KERNEL = 5
CLASSIFIER_HIDDEN = CHANNELS[5]       # 32

# ============================================================
# 训练参数
# ============================================================
BATCH_SIZE = 128
NUM_EPOCHS = 30
LEARNING_RATE = 3e-3
WEIGHT_DECAY = 1e-4
LABEL_SMOOTHING = 0.1
EARLY_STOPPING_PATIENCE = 10

# 学习率调度
LR_T_MAX = NUM_EPOCHS                 # 余弦退火周期
LR_ETA_MIN = 1e-6

# ============================================================
# 数据增强（仅训练集）
# ============================================================
NOISE_PROB = 0.8
TIME_SHIFT_SEC = 0.1                  # ±100ms
NOISE_VOLUME_MAX = 0.1
SILENCE_NUM = 500                     # _silence_ 样本数
UNKNOWN_PCT = 0.1                     # _unknown_ 采样比例

# ============================================================
# 设备
# ============================================================
if torch.cuda.is_available():
    DEVICE = torch.device("cuda")
elif torch.backends.mps.is_available():
    DEVICE = torch.device("mps")
else:
    DEVICE = torch.device("cpu")

# ============================================================
# 杂项
# ============================================================
NUM_WORKERS = 4 if DEVICE.type == "cuda" else 0
PIN_MEMORY = (DEVICE.type == "cuda")
