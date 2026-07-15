"""
阶段四：PC 端一致性验证 — Python vs C 推理
"""
import sys
import json
import struct
import subprocess
from pathlib import Path

import numpy as np
import torch

sys.path.insert(0, str(Path(__file__).resolve().parent))

import config
from model import BCResNet
from dataset import SpeechCommandsDataset, KWSIterableDataset
from torch.utils.data import DataLoader


def load_model():
    model = BCResNet(
        num_classes=config.NUM_CLASSES, mel_bins=config.MEL_BINS,
        channels=config.CHANNELS, stage_configs=config.STAGE_CONFIGS,
    )
    ckpt = torch.load(config.CHECKPOINT_DIR / "best_model.pth",
                      map_location="cpu", weights_only=False)
    model.load_state_dict(ckpt["model_state_dict"])
    model.eval()
    return model


def py_inference(model, waveform):
    """Python 推理，返回 (预测类别, logits)"""
    waveform = waveform.astype(np.float32)
    log_mel = SpeechCommandsDataset.extract_log_mel(waveform)
    log_mel = torch.from_numpy(log_mel.T[np.newaxis, np.newaxis, :, :])
    with torch.no_grad():
        logits = model(log_mel).squeeze(0).numpy()
    pred = int(logits.argmax())
    return pred, logits


def c_inference(waveform, exe_path):
    """C 推理，返回 (预测类别, 标签名, stdout)"""
    # 写入 PCM 文件
    pcm_path = config.OUTPUT_DIR / "test_audio.pcm"
    pcm_data = (waveform * 32767).clip(-32768, 32767).astype(np.int16)
    with open(pcm_path, "wb") as f:
        f.write(pcm_data.tobytes())

    result = subprocess.run(
        [str(exe_path), str(pcm_path)],
        capture_output=True, text=True, timeout=30,
    )
    stdout = result.stdout.strip()
    # 解析 "C result: N (label)"
    pred = -1
    label = "???"
    for line in stdout.split("\n"):
        if line.startswith("C result:"):
            parts = line.split()
            if len(parts) >= 3:
                pred = int(parts[2])
                label = parts[3].strip("()")
    return pred, label, stdout


def main():
    # 加载数据集
    print("加载数据...")
    data_path = SpeechCommandsDataset.download(target_dir=str(config.DATA_DIR.parent))
    dataset_obj = SpeechCommandsDataset(data_path)
    datasets, label_to_idx, noise_samples = dataset_obj.build_dataset(
        silence_num=config.SILENCE_NUM, unknown_pct=config.UNKNOWN_PCT,
    )
    idx_to_label = {v: k for k, v in label_to_idx.items()}
    test_ds = KWSIterableDataset(datasets["test"], noise_samples, augment=False)

    # 加载 PyTorch 模型
    py_model = load_model()

    # C 推理可执行文件
    c_exe = config.OUTPUT_DIR / "kws_test.exe"

    # 从每个类别取一个样本测试
    print("\n" + "=" * 70)
    print("逐类别一致性验证")
    print("=" * 70)

    match = 0
    total = 0
    per_class = {i: {"total": 0, "match": 0} for i in range(config.NUM_CLASSES)}

    for idx in range(config.NUM_CLASSES):
        # 找到该类别第一个样本
        sample = None
        for item in datasets["test"]:
            if item["label"] == idx:
                sample = item
                break
        if sample is None:
            print(f"  {idx_to_label[idx]:<15} 无样本，跳过")
            continue

        py_pred, py_logits = py_inference(py_model, sample["waveform"])
        c_pred, c_label, stdout = c_inference(sample["waveform"], c_exe)

        ok = py_pred == c_pred
        if ok: match += 1
        total += 1
        per_class[idx]["total"] += 1
        if ok: per_class[idx]["match"] += 1

        status = "OK" if ok else "MISMATCH!"
        print(f"  {idx_to_label[idx]:<15} "
              f"Python: {py_pred:>2}({idx_to_label[py_pred]:<10})  "
              f"C: {c_pred:>2}({c_label:<10})  [{status}]")

        if not ok:
            print(f"    Python logits: {py_logits}")
            print(f"    C stdout: {stdout}")

    print(f"\n一致性: {match}/{total} ({match/total*100:.1f}%)")

    # 整体测试集批量对比
    print("\n" + "=" * 70)
    print("批量对比（前 100 样本）")
    print("=" * 70)

    batch_match = 0
    n_test = min(100, len(datasets["test"]))
    for i in range(n_test):
        item = datasets["test"][i]
        py_pred, _ = py_inference(py_model, item["waveform"])
        c_pred, _, _ = c_inference(item["waveform"], c_exe)
        if py_pred == c_pred:
            batch_match += 1

    print(f"  批量一致性: {batch_match}/{n_test} ({batch_match/n_test*100:.1f}%)")

    if batch_match == n_test:
        print("\n  Python 与 C 推理完全一致！")
    else:
        print(f"\n  不一致: {n_test - batch_match} 个样本")

    return batch_match == n_test


if __name__ == "__main__":
    main()
