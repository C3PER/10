"""
阶段四：权重导出 — PyTorch → C 数组
"""
import sys
from pathlib import Path

import torch
import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))

import config
from model import BCResNet


def flatten(tensor):
    """Flatten tensor in row-major (C) order: last dim varies fastest"""
    return tensor.detach().cpu().numpy().ravel()


def format_array(arr, name, per_line=8):
    """Format a numpy array as a C double array literal"""
    lines = [f"const float {name}[{len(arr)}] = {{"]
    for i in range(0, len(arr), per_line):
        chunk = arr[i : i + per_line]
        line = "    " + ", ".join(f"{v:.9e}f" for v in chunk)
        if i + per_line < len(arr):
            line += ","
        lines.append(line)
    lines.append("};")
    return "\n".join(lines)


def export_model_weights(model, output_dir):
    """导出所有权重到 C 头文件和源文件"""
    state = model.state_dict()
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    all_declarations = []
    all_definitions = []
    all_macros = []

    # 维度宏
    all_macros.append("/* ===== 特征提取参数 ===== */")
    all_macros.append(f"#define FRM_LEN       {config.FRAME_LENGTH}")
    all_macros.append(f"#define WIN_SIZE      {config.WINDOW_SIZE}")
    all_macros.append(f"#define FFT_LEN       {config.FFT_SIZE}")
    all_macros.append(f"#define MELS_NUM      {config.MEL_BINS}")
    all_macros.append(f"#define SAMPLES_NUM   {config.SIGNAL_LENGTH}")
    all_macros.append(f"#define FREQ_NUM      {config.FREQ_NUM}")
    all_macros.append(f"#define CLASSES_NUM   {config.NUM_CLASSES}")
    all_macros.append(f"#define FS            {config.SAMPLE_RATE}")
    all_macros.append(f"#define PI            3.141592653589793")

    all_macros.append("\n/* ===== BC-ResNet 网络结构 ===== */")
    all_macros.append(f"#define CHANNEL0      {config.CHANNELS[0]}")
    all_macros.append(f"#define CHANNEL1      {config.CHANNELS[1]}")
    all_macros.append(f"#define CHANNEL2      {config.CHANNELS[2]}")
    all_macros.append(f"#define CHANNEL3      {config.CHANNELS[3]}")
    all_macros.append(f"#define CHANNEL4      {config.CHANNELS[4]}")
    all_macros.append(f"#define CHANNEL5      {config.CHANNELS[5]}")
    all_macros.append(f"#define STAGE_NUM     {len(config.STAGE_CONFIGS)}")

    weight_idx = 0
    all_macros.append(f"\n#define NUM_WEIGHT_ARRAYS  200  /* 实际数量见下方各组定义 */")

    # Helper to add a weight array
    def add_weight(tensor, name_prefix, comment=""):
        nonlocal weight_idx
        arr = flatten(tensor)
        all_declarations.append(f"extern const float {name_prefix}[{len(arr)}];  /* {comment} */")
        all_definitions.append(format_array(arr, name_prefix))
        all_definitions.append("")
        weight_idx += 1
        return weight_idx - 1

    # ================================================================
    # 1. Head Conv + BN
    # ================================================================
    all_declarations.append("\n/* ===== Head Conv2d (1→16, 5×5, stride_h=2) ===== */")
    add_weight(state["head_conv.weight"], "head_conv_w", f"({tuple(state['head_conv.weight'].shape)}) OIHW")
    add_weight(state["head_bn.weight"], "head_bn_w", "BN weight")
    add_weight(state["head_bn.bias"], "head_bn_b", "BN bias")
    add_weight(state["head_bn.running_mean"], "head_bn_mean", "BN mean")
    add_weight(state["head_bn.running_var"], "head_bn_var", "BN var")

    # ================================================================
    # 2. BC-ResBlock stages
    # ================================================================
    # 各阶段配置: (in_ch, out_ch, num_blocks, dilation, first_freq_stride)
    stage_configs = config.STAGE_CONFIGS
    block_idx = 0

    for s_idx, (cin, cout, n_blocks, dilation, first_stride) in enumerate(stage_configs):
        for b in range(n_blocks):
            b_cin = cin if b == 0 else cout
            b_stride = first_stride if b == 0 else 1
            use_skip = (b > 0)

            prefix = f"stage{s_idx}_block{b}"
            block = model.stages[s_idx].blocks[b]

            all_declarations.append(
                f"\n/* Stage {s_idx} Block {b}: {b_cin}→{cout}"
                f"{' stride=' + str(b_stride) if b_stride > 1 else ''}"
                f"{' skip' if use_skip else ''} */"
            )

            # 主路径 1×1 Conv + BN
            add_weight(block.main_conv1x1.weight, f"{prefix}_main_conv1x1_w",
                       f"({tuple(block.main_conv1x1.weight.shape)})")
            add_weight(block.main_bn1.weight, f"{prefix}_main_bn1_w", "BN w")
            add_weight(block.main_bn1.bias, f"{prefix}_main_bn1_b", "BN b")
            add_weight(block.main_bn1.running_mean, f"{prefix}_main_bn1_mean", "BN mean")
            add_weight(block.main_bn1.running_var, f"{prefix}_main_bn1_var", "BN var")

            # 主路径 3×1 DWConv + BN
            add_weight(block.main_dwconv.weight, f"{prefix}_main_dwconv_w",
                       f"({tuple(block.main_dwconv.weight.shape)})")
            add_weight(block.main_bn2.weight, f"{prefix}_main_bn2_w", "BN w")
            add_weight(block.main_bn2.bias, f"{prefix}_main_bn2_b", "BN b")
            add_weight(block.main_bn2.running_mean, f"{prefix}_main_bn2_mean", "BN mean")
            add_weight(block.main_bn2.running_var, f"{prefix}_main_bn2_var", "BN var")

            # 内部扩展 BN
            add_weight(block.expand_bn.weight, f"{prefix}_expand_bn_w",
                       f"({tuple(block.expand_bn.weight.shape)})")
            add_weight(block.expand_bn.bias, f"{prefix}_expand_bn_b", "BN b")
            add_weight(block.expand_bn.running_mean, f"{prefix}_expand_bn_mean", "BN mean")
            add_weight(block.expand_bn.running_var, f"{prefix}_expand_bn_var", "BN var")

            # 广播路径
            add_weight(block.broadcast_dwconv.weight, f"{prefix}_br_dwconv_w",
                       f"({tuple(block.broadcast_dwconv.weight.shape)})")
            add_weight(block.broadcast_bn.weight, f"{prefix}_br_bn_w", "BN w")
            add_weight(block.broadcast_bn.bias, f"{prefix}_br_bn_b", "BN b")
            add_weight(block.broadcast_bn.running_mean, f"{prefix}_br_bn_mean", "BN mean")
            add_weight(block.broadcast_bn.running_var, f"{prefix}_br_bn_var", "BN var")
            add_weight(block.broadcast_proj.weight, f"{prefix}_br_proj_w",
                       f"({tuple(block.broadcast_proj.weight.shape)})")

            block_idx += 1

    # ================================================================
    # 3. Final layers
    # ================================================================
    all_declarations.append("\n/* ===== Final DWConv 5×5 ===== */")
    add_weight(state["final_dwconv.weight"], "final_dwconv_w",
               f"({tuple(state['final_dwconv.weight'].shape)})")
    add_weight(state["final_dwconv_bn.weight"], "final_dwconv_bn_w", "BN w")
    add_weight(state["final_dwconv_bn.bias"], "final_dwconv_bn_b", "BN b")
    add_weight(state["final_dwconv_bn.running_mean"], "final_dwconv_bn_mean", "BN mean")
    add_weight(state["final_dwconv_bn.running_var"], "final_dwconv_bn_var", "BN var")

    all_declarations.append("\n/* ===== Expand Conv 1×1 ===== */")
    add_weight(state["expand_conv.weight"], "expand_conv_w",
               f"({tuple(state['expand_conv.weight'].shape)})")
    add_weight(state["expand_bn.weight"], "expand_bn_w", "BN w")
    add_weight(state["expand_bn.bias"], "expand_bn_b", "BN b")
    add_weight(state["expand_bn.running_mean"], "expand_bn_mean", "BN mean")
    add_weight(state["expand_bn.running_var"], "expand_bn_var", "BN var")

    all_declarations.append("\n/* ===== Classifier Conv 1×1 ===== */")
    add_weight(state["classifier.weight"], "classifier_w",
               f"({tuple(state['classifier.weight'].shape)})")
    add_weight(state["classifier.bias"], "classifier_b", "bias")

    # ================================================================
    # 写入文件
    # ================================================================
    header_path = output_dir / "kws_weights.h"
    source_path = output_dir / "kws_weights.c"

    with open(header_path, "w") as f:
        f.write("/* BC-ResNet 权重 — 由 export_weights.py 自动生成 */\n")
        f.write("#ifndef KWS_WEIGHTS_H\n#define KWS_WEIGHTS_H\n\n")
        f.write("#include <stddef.h>\n\n")
        f.write("\n".join(all_macros))
        f.write(f"\n\n/* 共 {weight_idx} 个权重数组 */\n")
        f.write("\n".join(all_declarations))
        f.write("\n\n#endif /* KWS_WEIGHTS_H */\n")

    with open(source_path, "w") as f:
        f.write('/* BC-ResNet 权重数据 — 由 export_weights.py 自动生成 */\n')
        f.write('#include "kws_weights.h"\n\n')
        f.write("\n".join(all_definitions))

    print(f"权重文件已导出:")
    print(f"  头文件: {header_path}  ({header_path.stat().st_size / 1024:.1f} KB)")
    print(f"  源文件: {source_path}  ({source_path.stat().st_size / 1024:.1f} KB)")
    print(f"  共 {weight_idx} 个权重数组")

    total_params = sum(len(flatten(p)) for p in state.values())
    print(f"  总参数量: {total_params:,} 个 float 值 ({total_params * 4 / 1024:.1f} KB)")


def main():
    print("加载模型...")
    model = BCResNet(
        num_classes=config.NUM_CLASSES,
        mel_bins=config.MEL_BINS,
        channels=config.CHANNELS,
        stage_configs=config.STAGE_CONFIGS,
    )
    checkpoint = torch.load(
        config.CHECKPOINT_DIR / "best_model.pth",
        map_location="cpu",
        weights_only=False,
    )
    model.load_state_dict(checkpoint["model_state_dict"])
    print(f"  Epoch {checkpoint['epoch']}, Val Acc: {checkpoint['val_acc']:.4f}")

    export_model_weights(model, config.OUTPUT_DIR)


if __name__ == "__main__":
    main()
