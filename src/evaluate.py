"""
阶段三：模型评估与分析 — BC-ResNet
"""
import sys
import json
import time
from pathlib import Path
from collections import defaultdict

import numpy as np
import torch
import torch.nn.functional as F
from torch.utils.data import DataLoader
from tqdm import tqdm
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

sys.path.insert(0, str(Path(__file__).resolve().parent))

import config
from model import BCResNet, count_parameters
from dataset import SpeechCommandsDataset, KWSIterableDataset


@torch.no_grad()
def evaluate(model, loader):
    """评估模型，返回预测结果和标签"""
    model.eval()
    all_preds = []
    all_labels = []

    for x, y in tqdm(loader, desc="Evaluating"):
        x = x.to(config.DEVICE)
        logits = model(x)
        preds = logits.argmax(dim=1).cpu().numpy()
        all_preds.extend(preds)
        all_labels.extend(y.numpy())

    return np.array(all_preds), np.array(all_labels)


def compute_confusion_matrix(y_true, y_pred, num_classes):
    cm = np.zeros((num_classes, num_classes), dtype=np.int64)
    for t, p in zip(y_true, y_pred):
        cm[t, p] += 1
    return cm


def compute_metrics(cm, class_names):
    """基于混淆矩阵计算 Precision / Recall / F1"""
    tp = np.diag(cm)
    fp = cm.sum(axis=0) - tp
    fn = cm.sum(axis=1) - tp

    precision = np.where(tp + fp > 0, tp / (tp + fp), 0)
    recall = np.where(tp + fn > 0, tp / (tp + fn), 0)
    f1 = np.where(precision + recall > 0, 2 * precision * recall / (precision + recall), 0)

    return {
        name: {"Precision": float(p), "Recall": float(r), "F1": float(f), "Samples": int(c)}
        for name, p, r, f, c in zip(class_names, precision, recall, f1, cm.sum(axis=1))
    }, float(precision.mean()), float(recall.mean()), float(f1.mean())


def plot_confusion_matrix(cm, class_names, save_path):
    """绘制归一化混淆矩阵"""
    cm_norm = cm.astype(np.float64) / cm.sum(axis=1, keepdims=True)
    cm_norm = np.nan_to_num(cm_norm)

    fig, ax = plt.subplots(figsize=(12, 10))
    im = ax.imshow(cm_norm, cmap="Blues", vmin=0, vmax=1)

    for i in range(len(class_names)):
        for j in range(len(class_names)):
            if cm_norm[i, j] > 0.5:
                color = "white"
            else:
                color = "black" if cm_norm[i, j] > 0.01 else "gray"
            text = f"{cm_norm[i, j]:.2f}" if cm_norm[i, j] >= 0.01 else (
                f"{cm[i, j]}" if cm[i, j] > 0 else "0"
            )
            ax.text(j, i, text, ha="center", va="center", fontsize=7, color=color)

    ax.set_xticks(range(len(class_names)))
    ax.set_yticks(range(len(class_names)))
    ax.set_xticklabels(class_names, rotation=45, ha="right", fontsize=9)
    ax.set_yticklabels(class_names, fontsize=9)
    ax.set_xlabel("Predicted", fontsize=11)
    ax.set_ylabel("True", fontsize=11)
    ax.set_title("Confusion Matrix (Normalized)", fontsize=13)
    plt.colorbar(im, ax=ax, shrink=0.8)
    plt.tight_layout()
    plt.savefig(save_path, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"混淆矩阵图已保存至 {save_path}")


def measure_latency(model, loader, n_runs=200):
    """测量推理延迟"""
    model.eval()
    # 预热
    for x, _ in loader:
        _ = model(x.to(config.DEVICE))
        break

    times = []
    with torch.no_grad():
        for i, (x, _) in enumerate(loader):
            if i >= n_runs:
                break
            x = x.to(config.DEVICE)
            torch.cuda.synchronize()
            t0 = time.perf_counter()
            _ = model(x)
            torch.cuda.synchronize()
            t1 = time.perf_counter()
            times.append((t1 - t0) * 1000)  # ms

    times = np.array(times)
    return {
        "mean_ms": float(times.mean()),
        "std_ms": float(times.std()),
        "p50_ms": float(np.percentile(times, 50)),
        "p95_ms": float(np.percentile(times, 95)),
        "p99_ms": float(np.percentile(times, 99)),
        "min_ms": float(times.min()),
        "max_ms": float(times.max()),
    }


def main():
    # 加载数据
    print("加载数据集...")
    data_path = SpeechCommandsDataset.download(target_dir=str(config.DATA_DIR.parent))
    dataset_obj = SpeechCommandsDataset(str(data_path))
    datasets, label_to_idx, noise_samples = dataset_obj.build_dataset(
        silence_num=config.SILENCE_NUM,
        unknown_pct=config.UNKNOWN_PCT,
    )
    idx_to_label = {v: k for k, v in label_to_idx.items()}
    class_names = [idx_to_label[i] for i in range(config.NUM_CLASSES)]

    test_ds = KWSIterableDataset(datasets["test"], noise_samples, augment=False)
    test_loader = DataLoader(
        test_ds, batch_size=config.BATCH_SIZE, shuffle=False,
        num_workers=config.NUM_WORKERS, pin_memory=config.PIN_MEMORY,
    )
    # 用 batch_size=1 测延迟
    latency_loader = DataLoader(
        test_ds, batch_size=1, shuffle=False,
        num_workers=0, pin_memory=False,
    )

    # 加载模型
    print("加载最佳模型...")
    model = BCResNet(
        num_classes=config.NUM_CLASSES, mel_bins=config.MEL_BINS,
        channels=config.CHANNELS, stage_configs=config.STAGE_CONFIGS,
    ).to(config.DEVICE)

    checkpoint = torch.load(config.CHECKPOINT_DIR / "best_model.pth",
                            map_location=config.DEVICE, weights_only=False)
    model.load_state_dict(checkpoint["model_state_dict"])
    model = model.to(config.DEVICE)

    total, _ = count_parameters(model)
    print(f"模型参数量: {total:,}")

    # ============================================================
    # 1. 评估
    # ============================================================
    print("\n" + "=" * 60)
    print("测试集评估")
    print("=" * 60)
    y_true, y_pred = evaluate(model, test_loader)
    acc = (y_true == y_pred).mean()
    print(f"\n总体 Accuracy: {acc:.4f} ({acc * 100:.2f}%)")

    # ============================================================
    # 2. 混淆矩阵
    # ============================================================
    cm = compute_confusion_matrix(y_true, y_pred, config.NUM_CLASSES)
    plot_confusion_matrix(cm, class_names, str(config.OUTPUT_DIR / "confusion_matrix.png"))

    # ============================================================
    # 3. Per-class 指标
    # ============================================================
    print("\n" + "=" * 60)
    print("各类别指标")
    print("=" * 60)
    class_metrics, macro_p, macro_r, macro_f1 = compute_metrics(cm, class_names)

    print(f"{'类别':<15} {'Precision':>10} {'Recall':>10} {'F1':>10} {'样本数':>8}")
    print("-" * 58)
    for name, m in class_metrics.items():
        print(f"{name:<15} {m['Precision']:>10.4f} {m['Recall']:>10.4f} {m['F1']:>10.4f} {m['Samples']:>8}")

    print("-" * 58)
    print(f"{'Macro Avg':<15} {macro_p:>10.4f} {macro_r:>10.4f} {macro_f1:>10.4f}")

    # 找出最易混淆的类别对
    print("\n易混淆 Top-5（非对角线）:")
    errs = []
    for i in range(config.NUM_CLASSES):
        for j in range(config.NUM_CLASSES):
            if i != j:
                errs.append((cm[i, j], class_names[i], class_names[j]))
    errs.sort(reverse=True)
    for count, true_label, pred_label in errs[:5]:
        print(f"  {true_label} → {pred_label}: {count} 次")

    # ============================================================
    # 4. 推理延迟
    # ============================================================
    print("\n" + "=" * 60)
    print("推理延迟（batch_size=1）")
    print("=" * 60)
    latency = measure_latency(model, latency_loader)
    for k, v in latency.items():
        print(f"  {k}: {v:.3f} ms")

    # ============================================================
    # 5. 保存报告
    # ============================================================
    report = {
        "overall_accuracy": float(acc),
        "best_epoch": checkpoint["epoch"],
        "best_val_acc": float(checkpoint["val_acc"]),
        "macro_precision": macro_p,
        "macro_recall": macro_r,
        "macro_f1": macro_f1,
        "class_metrics": {k: {kk: vv for kk, vv in v.items()} for k, v in class_metrics.items()},
        "latency": {k: round(v, 3) for k, v in latency.items()},
        "num_parameters": total,
    }

    report_path = config.OUTPUT_DIR / "evaluation_report.json"
    with open(report_path, "w") as f:
        json.dump(report, f, ensure_ascii=False, indent=2)
    print(f"\n评估报告已保存至 {report_path}")


if __name__ == "__main__":
    main()
