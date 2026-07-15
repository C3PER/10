"""
Google Speech Commands v2 数据集加载与预处理
项目制实验3 — 车内短语命令识别模块 (BC-ResNet)
"""

import os
import sys
import urllib.request
import tarfile
import shutil
import random
from pathlib import Path
from collections import Counter, defaultdict

import numpy as np
import librosa
from tqdm import tqdm

# ============================================================
# 全局配置（与指导书一致）
# ============================================================
SAMPLE_RATE = 16000
SIGNAL_LENGTH = 16000      # 1 秒音频
FRAME_LENGTH = 160         # 帧移 10ms
WINDOW_SIZE = 480          # 窗长 30ms
FFT_SIZE = 512
MEL_BINS = 40
FREQ_NUM = FFT_SIZE // 2 + 1  # 257

# URL for Google Speech Commands v2
DATASET_URL = "http://download.tensorflow.org/data/speech_commands_v0.02.tar.gz"

# 12 个目标类别
TARGET_CLASSES = [
    '_silence_', '_unknown_', 'down', 'go', 'left',
    'no', 'off', 'on', 'right', 'stop', 'up', 'yes'
]
# 10 个关键词（不含 _silence_ 和 _unknown_）
KEYWORDS = ['down', 'go', 'left', 'no', 'off', 'on', 'right', 'stop', 'up', 'yes']

# 数据集划分文件（数据集自带 speaker-disjoint 划分）
VALIDATION_LIST = "validation_list.txt"
TESTING_LIST = "testing_list.txt"


class SpeechCommandsDataset:
    """Google Speech Commands v2 数据集处理类"""

    def __init__(self, data_dir, cache_dir=None):
        """
        Args:
            data_dir: 数据集根目录 (speech_commands_v0.02 解压后的路径)
            cache_dir: 预处理后的缓存目录
        """
        self.data_dir = Path(data_dir)
        self.cache_dir = Path(cache_dir) if cache_dir else self.data_dir / "processed"
        self.cache_dir.mkdir(parents=True, exist_ok=True)

    # --------------------------------------------------------
    # 1. 下载与解压
    # --------------------------------------------------------
    @staticmethod
    def download(url=DATASET_URL, target_dir="./data"):
        """下载 Google Speech Commands v2 数据集"""
        target_dir = Path(target_dir)
        target_dir.mkdir(parents=True, exist_ok=True)
        tgz_path = target_dir / "speech_commands_v0.02.tar.gz"
        extracted_dir = target_dir / "speech_commands_v0.02"

        # 检查是否已解压（tar 包可能直接解压到 target_dir，也可能在子目录中）
        if extracted_dir.exists() and any(
            d.is_dir() for d in extracted_dir.iterdir()
            if d.name != 'processed' and not d.name.startswith('.')
        ):
            print(f"[SKIP] 数据集已解压到 {extracted_dir}")
            return str(extracted_dir)

        # tar 包可能直接解压到 target_dir 下（无父目录）
        bg_indicator = target_dir / "_background_noise_"
        if bg_indicator.exists():
            print(f"[SKIP] 数据集已解压到 {target_dir}（直接解压模式）")
            return str(target_dir)

        if not tgz_path.exists():
            print(f"正在下载数据集 (~2.4GB)...")
            print(f"URL: {url}")
            urllib.request.urlretrieve(url, tgz_path)
            print("下载完成。")

        print(f"正在解压到 {target_dir}...")
        with tarfile.open(tgz_path, "r:gz") as tar:
            tar.extractall(path=target_dir, filter='data')
        print("解压完成。")

        # 检查 tar 是否创建了子目录，否则数据直接在 target_dir 中
        if extracted_dir.exists():
            return str(extracted_dir)
        else:
            return str(target_dir)

    # --------------------------------------------------------
    # 2. 构建文件索引
    # --------------------------------------------------------
    def build_index(self):
        """扫描数据集目录，构建 (文件路径, 标签) 索引"""
        audio_files = []
        labels = []
        speakers = []

        # 读取划分文件（统一使用正斜杠，兼容 Windows）
        val_files = set()
        test_files = set()
        val_path = self.data_dir / VALIDATION_LIST
        test_path = self.data_dir / TESTING_LIST
        if val_path.exists():
            with open(val_path, 'r') as f:
                val_files = {line.strip().replace('\\', '/') for line in f if line.strip()}
        if test_path.exists():
            with open(test_path, 'r') as f:
                test_files = {line.strip().replace('\\', '/') for line in f if line.strip()}

        # 扫描所有音频文件
        all_dirs = sorted(d for d in self.data_dir.iterdir() if d.is_dir())
        for class_dir in all_dirs:
            label = class_dir.name
            if label == '_background_noise_':
                continue  # 背景噪音单独处理
            if label not in KEYWORDS:
                continue  # 只保留目标关键词

            for wav_path in class_dir.glob("*.wav"):
                rel_path = str(wav_path.relative_to(self.data_dir)).replace('\\', '/')
                # 确定划分
                if rel_path in val_files:
                    split = "val"
                elif rel_path in test_files:
                    split = "test"
                else:
                    split = "train"

                audio_files.append(str(wav_path))
                labels.append(label)
                speakers.append((split, rel_path))

        return audio_files, labels, speakers

    # --------------------------------------------------------
    # 3. 音频预处理
    # --------------------------------------------------------
    def load_and_preprocess(self, file_path):
        """加载 wav 文件，统一长度 16000 采样点"""
        waveform, sr = librosa.load(file_path, sr=SAMPLE_RATE, mono=True)
        # 统一到 1 秒长度
        if len(waveform) > SIGNAL_LENGTH:
            waveform = waveform[:SIGNAL_LENGTH]
        elif len(waveform) < SIGNAL_LENGTH:
            waveform = np.pad(waveform, (0, SIGNAL_LENGTH - len(waveform)))
        return waveform.astype(np.float32)

    @staticmethod
    def extract_log_mel(waveform):
        """从波形提取 Log-Mel 频谱图（与指导书 C 代码对齐）"""
        # 对称 padding
        pad_len = WINDOW_SIZE // 2
        padded = np.pad(waveform, (pad_len, pad_len), mode='reflect')

        # 分帧
        num_frames = len(waveform) // FRAME_LENGTH + 1
        frames = np.zeros((num_frames, WINDOW_SIZE), dtype=np.float32)
        for i in range(num_frames):
            start = i * FRAME_LENGTH
            frames[i] = padded[start:start + WINDOW_SIZE]

        # Hann 窗
        window = 0.5 * (1 - np.cos(2 * np.pi * np.arange(WINDOW_SIZE) / WINDOW_SIZE))
        windowed = frames * window.astype(np.float32)

        # FFT
        spec = np.fft.rfft(windowed, n=FFT_SIZE, axis=1)
        power_spec = np.abs(spec) ** 2  # shape: (num_frames, FREQ_NUM)

        # Mel 滤波器组
        mel_fb = SpeechCommandsDataset._mel_filterbank(MEL_BINS, FREQ_NUM, SAMPLE_RATE)
        mel_spec = power_spec @ mel_fb  # shape: (num_frames, MEL_BINS)

        # Log
        log_mel = np.log(mel_spec + 1e-6)

        return log_mel.astype(np.float32)  # shape: (num_frames, MEL_BINS)

    @staticmethod
    def _mel_filterbank(n_mels, n_freq, sr):
        """生成 Mel 滤波器组矩阵 (n_freq, n_mels)"""
        f_min, f_max = 0, sr / 2
        mel_min = 2595.0 * np.log10(1.0 + f_min / 700.0)
        mel_max = 2595.0 * np.log10(1.0 + f_max / 700.0)

        mel_pts = np.linspace(mel_min, mel_max, n_mels + 2)
        freq_pts = 700.0 * (10 ** (mel_pts / 2595.0) - 1.0)

        all_freqs = np.linspace(0, f_max, n_freq)
        f_diff = np.diff(freq_pts)

        filter_bank = np.zeros((n_freq, n_mels))
        for i in range(n_mels):
            slopes_low = (all_freqs - freq_pts[i]) / f_diff[i]
            slopes_high = (freq_pts[i + 2] - all_freqs) / f_diff[i + 1]
            filter_bank[:, i] = np.maximum(0.0, np.minimum(slopes_low, slopes_high))

        return filter_bank

    # --------------------------------------------------------
    # 4. 数据增强（仅训练集）
    # --------------------------------------------------------
    @staticmethod
    def augment(waveform, noise_samples=None, p_noise=0.8):
        """
        数据增强：
        1. 时间偏移：随机 ±100ms
        2. 背景噪声混合
        """
        sr = SAMPLE_RATE
        shift_max = int(0.1 * sr)  # ±100ms
        shift = random.randint(-shift_max, shift_max)

        if shift > 0:
            augmented = np.pad(waveform, (shift, 0), mode='constant')[:len(waveform)]
        elif shift < 0:
            augmented = np.pad(waveform, (0, -shift), mode='constant')[-len(waveform):]
        else:
            augmented = waveform.copy()

        # 背景噪声混合
        if noise_samples is not None and random.random() < p_noise:
            noise = random.choice(noise_samples)
            # 截取/拼接噪声到目标长度
            if len(noise) < len(augmented):
                repeats = len(augmented) // len(noise) + 1
                noise = np.tile(noise, repeats)
            start = random.randint(0, len(noise) - len(augmented))
            noise_segment = noise[start:start + len(augmented)]
            volume = random.uniform(0, 0.1)
            augmented = augmented + volume * noise_segment

        return augmented.astype(np.float32)

    # --------------------------------------------------------
    # 5. 构建完整数据集
    # --------------------------------------------------------
    def build_dataset(self, silence_num=500, unknown_pct=0.1):
        """
        构建完整的训练/验证/测试集。

        Args:
            silence_num: _silence_ 类别样本数（从 background_noise 中截取）
            unknown_pct: _unknown_ 类别占非目标词的比例

        Returns:
            dict: {'train': [(waveform, label_idx)], 'val': [...], 'test': [...]}
        """
        print("扫描数据集文件...")
        audio_files, labels, speakers = self.build_index()

        # 建立划分映射
        label_to_idx = {name: i for i, name in enumerate(TARGET_CLASSES)}

        # 按划分收集
        splits = defaultdict(list)
        for fpath, label, (split, rel_path) in zip(audio_files, labels, speakers):
            splits[split].append((fpath, label))

        print(f"  训练集文件: {len(splits['train'])}")
        print(f"  验证集文件: {len(splits['val'])}")
        print(f"  测试集文件: {len(splits['test'])}")

        # 加载背景噪音
        bg_dir = self.data_dir / "_background_noise_"
        noise_samples = []
        if bg_dir.exists():
            for wav_path in bg_dir.glob("*.wav"):
                noise, _ = librosa.load(wav_path, sr=SAMPLE_RATE)
                noise_samples.append(noise)
            print(f"  背景噪音文件: {len(noise_samples)}")

        # ---- 处理 _silence_ 和 _unknown_ ----
        # _unknown_: 从其他非目标词中采样
        other_dirs = [d for d in self.data_dir.iterdir()
                      if d.is_dir() and d.name != '_background_noise_' and d.name not in KEYWORDS]
        other_files = []
        for d in other_dirs:
            other_files.extend(str(p) for p in d.glob("*.wav"))
        random.shuffle(other_files)

        unknown_count = max(int(len(other_files) * unknown_pct), 100)
        unknown_files = random.sample(other_files, unknown_count)

        # 将 unknown 按比例分配到各划分
        n_train = len(splits['train'])
        n_val = len(splits['val'])
        n_test = len(splits['test'])
        n_total = n_train + n_val + n_test

        n_unknown_train = int(unknown_count * n_train / n_total)
        n_unknown_val = int(unknown_count * n_val / n_total)
        n_unknown_test = unknown_count - n_unknown_train - n_unknown_val

        datasets = {'train': [], 'val': [], 'test': []}

        # 处理关键词
        for split_name, file_list in splits.items():
            print(f"\n处理 {split_name} 集 ({len(file_list)} 文件)...")
            for fpath, label in tqdm(file_list):
                waveform = self.load_and_preprocess(fpath)
                datasets[split_name].append({
                    'waveform': waveform,
                    'label': label_to_idx[label],
                    'label_name': label
                })

        # 处理 _unknown_
        unknown_split_counts = {'train': n_unknown_train, 'val': n_unknown_val, 'test': n_unknown_test}
        for split_name, count in unknown_split_counts.items():
            start_idx = sum(unknown_split_counts[k] for k in ['train', 'val', 'test'] if k < split_name)
            for i in range(count):
                fpath = unknown_files[start_idx + i]
                waveform = self.load_and_preprocess(fpath)
                datasets[split_name].append({
                    'waveform': waveform,
                    'label': label_to_idx['_unknown_'],
                    'label_name': '_unknown_'
                })

        # 处理 _silence_（从背景噪音中随机截取）
        silence_splits = {}
        for split_name, count in zip(['train', 'val', 'test'],
                                     [int(silence_num * n_train / n_total),
                                      int(silence_num * n_val / n_total),
                                      silence_num - int(silence_num * n_train / n_total) - int(
                                          silence_num * n_val / n_total)]):
            silence_splits[split_name] = count

        for split_name, count in silence_splits.items():
            for _ in range(count):
                noise = random.choice(noise_samples)
                if len(noise) < SIGNAL_LENGTH:
                    repeats = SIGNAL_LENGTH // len(noise) + 1
                    noise = np.tile(noise, repeats)
                start = random.randint(0, len(noise) - SIGNAL_LENGTH)
                silence_waveform = noise[start:start + SIGNAL_LENGTH].astype(np.float32)
                datasets[split_name].append({
                    'waveform': silence_waveform,
                    'label': label_to_idx['_silence_'],
                    'label_name': '_silence_'
                })

        # 打乱
        for split_name in datasets:
            random.shuffle(datasets[split_name])

        return datasets, label_to_idx, noise_samples

    # --------------------------------------------------------
    # 6. 统计报告
    # --------------------------------------------------------
    @staticmethod
    def print_statistics(datasets, label_to_idx):
        """打印数据集统计报告"""
        idx_to_label = {v: k for k, v in label_to_idx.items()}

        print("\n" + "=" * 60)
        print("数据集统计报告")
        print("=" * 60)

        for split_name in ['train', 'val', 'test']:
            data = datasets[split_name]
            labels = [d['label'] for d in data]
            counter = Counter(labels)

            print(f"\n{split_name.upper()} 集: 总计 {len(data)} 样本")
            print(f"{'类别':<15} {'样本数':>8} {'占比':>8}")
            print("-" * 35)
            for idx in range(len(TARGET_CLASSES)):
                count = counter.get(idx, 0)
                pct = count / len(data) * 100 if len(data) > 0 else 0
                print(f"{idx_to_label[idx]:<15} {count:>8} {pct:>7.1f}%")

        print("\n" + "=" * 60)


# ============================================================
# PyTorch Dataset 封装
# ============================================================
class KWSIterableDataset:
    """
    迭代式数据集，在线提取 Log-Mel 特征并进行数据增强。
    适合大数据集场景，避免将所有数据预加载到内存。
    """

    def __init__(self, data_list, noise_samples=None, augment=False):
        self.data_list = data_list
        self.noise_samples = noise_samples
        self.augment = augment

    def __len__(self):
        return len(self.data_list)

    def __getitem__(self, idx):
        item = self.data_list[idx]
        waveform = item['waveform'].copy()

        if self.augment:
            waveform = SpeechCommandsDataset.augment(waveform, self.noise_samples)

        log_mel = SpeechCommandsDataset.extract_log_mel(waveform)
        # log_mel shape: (num_frames, MEL_BINS) = (101, 40)
        # 转换为 (1, MEL_BINS, num_frames) 适配 Conv2d
        log_mel = log_mel.T[np.newaxis, :, :]  # (1, 40, 101)
        return log_mel.astype(np.float32), item['label']


if __name__ == "__main__":
    # ---- 测试：下载数据集 ----
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--data_dir", type=str, default="./data")
    parser.add_argument("--download", action="store_true", default=True)
    args = parser.parse_args()

    data_path = SpeechCommandsDataset.download(target_dir=args.data_dir)
    dataset = SpeechCommandsDataset(data_path)

    datasets, label_to_idx, noise_samples = dataset.build_dataset()
    dataset.print_statistics(datasets, label_to_idx)

    # 测试特征提取
    print("\n[测试] 提取一条音频的 Log-Mel 谱...")
    sample = datasets['train'][0]
    log_mel = SpeechCommandsDataset.extract_log_mel(sample['waveform'])
    print(f"Log-Mel 谱维度: {log_mel.shape}  (应为 ~101×40)")
