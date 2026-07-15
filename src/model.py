"""
BC-ResNet: Broadcasted Residual Learning for Efficient Keyword Spotting
项目制实验3 — 车内短语命令识别模块

BCResBlock 结构:
  主路径: 1×1(Cin→Cout) → BN → ReLU → 3×1 DWConv(stride) → BN → ReLU
          → [内部: freq→chan reshape(×5) → BN → reshape back]
  广播路径: AvgPool(freq→1) → 1×3 DWConv(dil) → BN → Sigmoid → 1×1(Cout→Cout)
  输出: main + broadcast (+ skip) → ReLU
"""
import torch
import torch.nn as nn
import torch.nn.functional as F


class BCResBlock(nn.Module):
    def __init__(self, in_channels, out_channels, freq_stride=1, dilation=1,
                 expansion=5, use_skip=False):
        super().__init__()
        self.expansion = expansion
        self.use_skip = use_skip
        self.freq_stride = freq_stride

        # 主路径
        self.main_conv1x1 = nn.Conv2d(in_channels, out_channels, kernel_size=1, bias=False)
        self.main_bn1 = nn.BatchNorm2d(out_channels)

        self.main_dwconv = nn.Conv2d(
            out_channels, out_channels,
            kernel_size=(3, 1), stride=(freq_stride, 1), padding=(1, 0),
            groups=out_channels, bias=False,
        )
        self.main_bn2 = nn.BatchNorm2d(out_channels)

        # 内部频率→通道扩展 BN（C*exp 个通道）
        expanded = out_channels * expansion
        self.expand_bn = nn.BatchNorm2d(expanded)

        # 广播残差路径
        self.broadcast_dwconv = nn.Conv2d(
            out_channels, out_channels,
            kernel_size=(1, 3), padding=(0, dilation), dilation=(1, dilation),
            groups=out_channels, bias=False,
        )
        self.broadcast_bn = nn.BatchNorm2d(out_channels)
        self.broadcast_proj = nn.Conv2d(out_channels, out_channels, kernel_size=1, bias=False)

    def forward(self, x):
        identity = x

        out = self.main_conv1x1(x)
        out = F.relu(self.main_bn1(out), inplace=True)
        out = self.main_dwconv(out)
        out = F.relu(self.main_bn2(out), inplace=True)

        B, C, H, W = out.shape
        exp = self.expansion

        # 内部扩展: (B, C, H, W) → (B, C*exp, H/exp, W) → BN → 回到 (B, C, H, W)
        out = out.reshape(B, C, H // exp, exp, W)
        out = out.permute(0, 1, 3, 2, 4).reshape(B, C * exp, H // exp, W)
        out = self.expand_bn(out)
        # reshape back
        out = out.reshape(B, C, exp, H // exp, W)
        out = out.permute(0, 1, 3, 2, 4).reshape(B, C, H, W)
        out_main = F.relu(out, inplace=True)

        # 广播残差路径
        br = F.adaptive_avg_pool2d(out_main, (1, W))
        br = self.broadcast_dwconv(br)
        br = self.broadcast_bn(br)
        br = torch.sigmoid(br)
        br = self.broadcast_proj(br)

        out = out_main + br
        if self.use_skip and identity.shape[1] == C:
            if identity.shape[2:] != out.shape[2:]:
                identity = F.adaptive_avg_pool2d(identity, out.shape[2:])
            out = out + identity
        return F.relu(out, inplace=True)


class BCResStage(nn.Module):
    def __init__(self, in_channels, out_channels, num_blocks, dilation, first_freq_stride,
                 expansion=5):
        super().__init__()
        blocks = []
        for i in range(num_blocks):
            cin = in_channels if i == 0 else out_channels
            stride = first_freq_stride if i == 0 else 1
            blocks.append(BCResBlock(cin, out_channels, freq_stride=stride,
                                     dilation=dilation, expansion=expansion,
                                     use_skip=(i > 0)))
        self.blocks = nn.Sequential(*blocks)

    def forward(self, x):
        return self.blocks(x)


class BCResNet(nn.Module):
    def __init__(self, num_classes=12, mel_bins=40,
                 channels=(16, 8, 12, 16, 20, 32),
                 stage_configs=None):
        super().__init__()

        c0, c1, c2, c3, c4, c5 = channels

        # 入口卷积: 1×40×W → 16×20×W
        self.head_conv = nn.Conv2d(1, c0, kernel_size=5, stride=(2, 1), padding=(2, 2), bias=False)
        self.head_bn = nn.BatchNorm2d(c0)

        if stage_configs is None:
            stage_configs = [
                (c0, c1, 2, 1, 1),
                (c1, c2, 2, 1, 2),
                (c2, c3, 4, 2, 2),
                (c3, c4, 4, 4, 1),
            ]

        self.stages = nn.ModuleList()
        for cin, cout, n, dil, stride in stage_configs:
            self.stages.append(BCResStage(cin, cout, n, dil, stride))

        # 末端: 20×5×W → 20×1×W
        self.final_dwconv = nn.Conv2d(
            c4, c4, kernel_size=5, stride=(5, 1), padding=(2, 2),
            groups=c4, bias=False,
        )
        self.final_dwconv_bn = nn.BatchNorm2d(c4)

        # 升维: 20×1×W → 32×1×W
        self.expand_conv = nn.Conv2d(c4, c5, kernel_size=1, bias=False)
        self.expand_bn = nn.BatchNorm2d(c5)

        # 分类器
        self.classifier = nn.Conv2d(c5, num_classes, kernel_size=1)

        self._init_weights()

    def _init_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.constant_(m.weight, 1)
                nn.init.constant_(m.bias, 0)

    def forward(self, x):
        x = self.head_conv(x)
        x = F.relu(self.head_bn(x), inplace=True)

        for stage in self.stages:
            x = stage(x)

        x = self.final_dwconv(x)
        x = self.final_dwconv_bn(x)
        x = F.relu(x, inplace=True)

        x = self.expand_conv(x)
        x = F.relu(self.expand_bn(x), inplace=True)

        x = F.adaptive_avg_pool2d(x, (1, 1))
        x = self.classifier(x)
        return x.view(x.size(0), -1)


def count_parameters(model):
    total = sum(p.numel() for p in model.parameters())
    trainable = sum(p.numel() for p in model.parameters() if p.requires_grad)
    return total, trainable


if __name__ == "__main__":
    model = BCResNet(num_classes=12, mel_bins=40)
    total, trainable = count_parameters(model)
    print(f"BC-ResNet 参数量: {total:,} (可训练: {trainable:,})")

    x = torch.randn(2, 1, 40, 101)
    with torch.no_grad():
        y = model(x)
    print(f"输入:  {x.shape}")
    print(f"输出:  {y.shape}   (应为 [2, 12])")

    print("\n--- 逐层形状 ---")
    with torch.no_grad():
        out = model.head_conv(x)
        out = F.relu(model.head_bn(out))
        print(f"Head Conv:       {tuple(out.shape)}")

        for i, stage in enumerate(model.stages):
            out = stage(out)
            print(f"Stage {i}:        {tuple(out.shape)}")

        out = model.final_dwconv(out)
        out = F.relu(model.final_dwconv_bn(out))
        print(f"Final DWConv:    {tuple(out.shape)}")

        out = model.expand_conv(out)
        out = F.relu(model.expand_bn(out))
        print(f"Expand Conv:     {tuple(out.shape)}")

        out = F.adaptive_avg_pool2d(out, (1, 1))
        print(f"Global AvgPool:  {tuple(out.shape)}")

        out = model.classifier(out)
        print(f"Classifier:      {tuple(out.shape)}")
