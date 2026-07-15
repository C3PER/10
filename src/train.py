"""
BC-ResNet 训练脚本
项目制实验3 — 车内短语命令识别模块
"""
import os
import sys
import time
import json
import random
from pathlib import Path
from collections import defaultdict

import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from tqdm import tqdm

# 添加项目路径
sys.path.insert(0, str(Path(__file__).resolve().parent))

import config
from model import BCResNet, count_parameters
from dataset import SpeechCommandsDataset, KWSIterableDataset


def set_seed(seed=42):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)
        torch.backends.cudnn.deterministic = True
        torch.backends.cudnn.benchmark = True


def prepare_data():
    """加载数据集并创建 DataLoader"""
    data_path = SpeechCommandsDataset.download(target_dir=str(config.DATA_DIR.parent))
    dataset = SpeechCommandsDataset(data_path)

    datasets, label_to_idx, noise_samples = dataset.build_dataset(
        silence_num=config.SILENCE_NUM,
        unknown_pct=config.UNKNOWN_PCT,
    )
    dataset.print_statistics(datasets, label_to_idx)

    train_ds = KWSIterableDataset(datasets['train'], noise_samples, augment=True)
    val_ds = KWSIterableDataset(datasets['val'], noise_samples, augment=False)
    test_ds = KWSIterableDataset(datasets['test'], noise_samples, augment=False)

    train_loader = DataLoader(
        train_ds, batch_size=config.BATCH_SIZE, shuffle=True,
        num_workers=config.NUM_WORKERS, pin_memory=config.PIN_MEMORY,
        drop_last=True,
    )
    val_loader = DataLoader(
        val_ds, batch_size=config.BATCH_SIZE, shuffle=False,
        num_workers=config.NUM_WORKERS, pin_memory=config.PIN_MEMORY,
    )
    test_loader = DataLoader(
        test_ds, batch_size=config.BATCH_SIZE, shuffle=False,
        num_workers=config.NUM_WORKERS, pin_memory=config.PIN_MEMORY,
    )

    print(f"\nDataLoader: train={len(train_loader)} batches, "
          f"val={len(val_loader)} batches, test={len(test_loader)} batches")
    return train_loader, val_loader, test_loader, label_to_idx


def build_model():
    """构建 BC-ResNet 模型、损失函数、优化器、调度器"""
    model = BCResNet(
        num_classes=config.NUM_CLASSES,
        mel_bins=config.MEL_BINS,
        channels=config.CHANNELS,
        stage_configs=config.STAGE_CONFIGS,
    )
    model = model.to(config.DEVICE)

    total, trainable = count_parameters(model)
    print(f"\nBC-ResNet: {total:,} 参数 ({trainable:,} 可训练)")

    criterion = nn.CrossEntropyLoss(label_smoothing=config.LABEL_SMOOTHING)

    optimizer = optim.AdamW(
        model.parameters(),
        lr=config.LEARNING_RATE,
        weight_decay=config.WEIGHT_DECAY,
    )

    scheduler = optim.lr_scheduler.CosineAnnealingLR(
        optimizer,
        T_max=config.LR_T_MAX,
        eta_min=config.LR_ETA_MIN,
    )

    return model, criterion, optimizer, scheduler


def train_one_epoch(model, loader, criterion, optimizer, epoch):
    """训练一个 epoch"""
    model.train()
    running_loss = 0.0
    correct = 0
    total = 0

    pbar = tqdm(loader, desc=f"Epoch {epoch:2d} [Train]")
    for x, y in pbar:
        x, y = x.to(config.DEVICE), y.to(config.DEVICE)

        optimizer.zero_grad()
        logits = model(x)
        loss = criterion(logits, y)
        loss.backward()
        optimizer.step()

        running_loss += loss.item() * x.size(0)
        preds = logits.argmax(dim=1)
        correct += (preds == y).sum().item()
        total += y.size(0)

        pbar.set_postfix(loss=f"{loss.item():.3f}", acc=f"{correct / total:.3f}")

    epoch_loss = running_loss / total
    epoch_acc = correct / total
    return epoch_loss, epoch_acc


@torch.no_grad()
def validate(model, loader, criterion):
    """验证/测试"""
    model.eval()
    running_loss = 0.0
    correct = 0
    total = 0

    for x, y in loader:
        x, y = x.to(config.DEVICE), y.to(config.DEVICE)
        logits = model(x)
        loss = criterion(logits, y)

        running_loss += loss.item() * x.size(0)
        preds = logits.argmax(dim=1)
        correct += (preds == y).sum().item()
        total += y.size(0)

    epoch_loss = running_loss / total
    epoch_acc = correct / total
    return epoch_loss, epoch_acc


def save_checkpoint(model, optimizer, scheduler, epoch, val_acc, path):
    torch.save({
        'epoch': epoch,
        'model_state_dict': model.state_dict(),
        'optimizer_state_dict': optimizer.state_dict(),
        'scheduler_state_dict': scheduler.state_dict(),
        'val_acc': val_acc,
    }, path)


def plot_curves(history, save_dir):
    """绘制 loss 和 accuracy 曲线"""
    epochs = range(1, len(history['train_loss']) + 1)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))

    ax1.plot(epochs, history['train_loss'], 'b-', label='Train Loss')
    ax1.plot(epochs, history['val_loss'], 'r-', label='Val Loss')
    ax1.set_xlabel('Epoch')
    ax1.set_ylabel('Loss')
    ax1.set_title('Loss Curve')
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    ax2.plot(epochs, history['train_acc'], 'b-', label='Train Acc')
    ax2.plot(epochs, history['val_acc'], 'r-', label='Val Acc')
    ax2.set_xlabel('Epoch')
    ax2.set_ylabel('Accuracy')
    ax2.set_title('Accuracy Curve')
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    path = os.path.join(save_dir, "training_curves.png")
    plt.savefig(path, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"曲线图已保存至 {path}")


def main():
    set_seed(42)
    print(f"设备: {config.DEVICE}")
    print(f"批次大小: {config.BATCH_SIZE} | 学习率: {config.LEARNING_RATE} | Epochs: {config.NUM_EPOCHS}")

    # 准备数据
    print("\n" + "=" * 60)
    print("准备数据集...")
    print("=" * 60)
    train_loader, val_loader, test_loader, label_to_idx = prepare_data()

    # 保存类别映射
    idx_to_label = {v: k for k, v in label_to_idx.items()}
    with open(config.OUTPUT_DIR / "label_map.json", "w") as f:
        json.dump(idx_to_label, f, ensure_ascii=False, indent=2)

    # 构建模型
    print("\n" + "=" * 60)
    print("构建模型...")
    print("=" * 60)
    model, criterion, optimizer, scheduler = build_model()

    # 训练
    print("\n" + "=" * 60)
    print("开始训练")
    print("=" * 60)
    start_time = time.time()

    history = defaultdict(list)
    best_val_acc = 0.0
    best_epoch = 0
    patience_counter = 0

    for epoch in range(1, config.NUM_EPOCHS + 1):
        train_loss, train_acc = train_one_epoch(model, train_loader, criterion, optimizer, epoch)
        val_loss, val_acc = validate(model, val_loader, criterion)

        current_lr = optimizer.param_groups[0]['lr']
        scheduler.step()

        history['train_loss'].append(train_loss)
        history['train_acc'].append(train_acc)
        history['val_loss'].append(val_loss)
        history['val_acc'].append(val_acc)
        history['lr'].append(current_lr)

        print(f"Epoch {epoch:2d}/{config.NUM_EPOCHS} | "
              f"Train Loss: {train_loss:.4f} Acc: {train_acc:.4f} | "
              f"Val Loss: {val_loss:.4f} Acc: {val_acc:.4f} | "
              f"LR: {current_lr:.2e}")

        # 保存最佳模型
        if val_acc > best_val_acc:
            best_val_acc = val_acc
            best_epoch = epoch
            patience_counter = 0
            save_checkpoint(model, optimizer, scheduler, epoch, val_acc,
                            config.CHECKPOINT_DIR / "best_model.pth")
            print(f"  >> 最佳模型已保存 (Val Acc: {val_acc:.4f})")
        else:
            patience_counter += 1

        # 定期保存
        if epoch % 10 == 0:
            save_checkpoint(model, optimizer, scheduler, epoch, val_acc,
                            config.CHECKPOINT_DIR / f"checkpoint_epoch{epoch:02d}.pth")

        # Early stopping
        if patience_counter >= config.EARLY_STOPPING_PATIENCE:
            print(f"\nEarly stopping at epoch {epoch} (patience={config.EARLY_STOPPING_PATIENCE})")
            break

    elapsed = time.time() - start_time
    print(f"\n训练完成，耗时 {elapsed / 60:.1f} 分钟")
    print(f"最佳验证准确率: {best_val_acc:.4f} (Epoch {best_epoch})")

    # 保存历史记录
    with open(config.LOG_DIR / "training_history.json", "w") as f:
        json.dump({k: [float(v) for v in vals] for k, vals in history.items()}, f, indent=2)

    # 绘制曲线
    plot_curves(history, config.OUTPUT_DIR)

    # 测试集评估
    print("\n" + "=" * 60)
    print("测试集评估")
    print("=" * 60)
    checkpoint = torch.load(config.CHECKPOINT_DIR / "best_model.pth", map_location=config.DEVICE)
    model.load_state_dict(checkpoint['model_state_dict'])
    test_loss, test_acc = validate(model, test_loader, criterion)
    print(f"测试集 Loss: {test_loss:.4f}  Accuracy: {test_acc:.4f}")

    return model, history, test_acc


if __name__ == "__main__":
    main()
