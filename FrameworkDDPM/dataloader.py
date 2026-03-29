from torchvision import transforms
from torch.utils.data import DataLoader
import numpy as np
import torch
import torchvision
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import filedialog
import os

def select_dataset_dir(root: tk.Tk, split: str) -> str:
    """
    弹出文件夹选择对话框，让用户选择指定数据集分割的路径。
    若用户取消或所选路径无效，则循环提示重新选择。

    Args:
        root:  已初始化并隐藏的 tkinter 根窗口。
        split: 数据集类型名称，如 'train' 或 'test'。

    Returns:
        用户选择的有效目录路径字符串。
    """
    while True:
        print(f"正在等待选择 {split} 数据集路径...")

        selected = filedialog.askdirectory(
            title=f"请选择 {split} 目录（应包含类别子文件夹，如 cls0）",
            parent=root,
        )

        if not selected:
            print(f"  ⚠ 未选择路径，请重新选择 {split} 目录。")
            continue

        subdirs = [d for d in os.listdir(selected) if os.path.isdir(os.path.join(selected, d))]
        if len(subdirs) == 0:
            print(f"  ⚠ 所选目录不包含任何子文件夹，路径无效，请重新选择。")
            continue

        print(f"{split.capitalize()} 数据集已选择: {selected}")
        return selected


def load_transformed_dataset(img_size=256, batch_size=128) -> DataLoader:
    # Load dataset and perform data transformations
    data_transforms = [
        transforms.Resize((img_size, img_size)),
        transforms.ToTensor(),  # Scales data into [0,1]
        transforms.Lambda(lambda t: (t * 2) - 1),  # Scale between [-1, 1]
    ]
    data_transform = transforms.Compose(data_transforms)

    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)

    try:
        train_dir = select_dataset_dir(root, "train")
        test_dir  = select_dataset_dir(root, "test")
    finally:
        root.destroy()  # 无论是否抛出异常，都确保窗口被销毁


    train = torchvision.datasets.ImageFolder(root=train_dir, transform=data_transform)
    test  = torchvision.datasets.ImageFolder(root=test_dir,  transform=data_transform)
    dataset = torch.utils.data.ConcatDataset([train, test])

    return DataLoader(dataset, batch_size=batch_size, shuffle=True, drop_last=True)


def show_tensor_image(image):
    # Reverse the data transformations
    reverse_transforms = transforms.Compose(
        [
            transforms.Lambda(lambda t: (t + 1) / 2),
            transforms.Lambda(lambda t: t.permute(1, 2, 0)),  # CHW to HWC
            transforms.Lambda(lambda t: t * 255.0),
            transforms.Lambda(lambda t: t.numpy().astype(np.uint8)),
            transforms.ToPILImage(),
        ]
    )

    # Take first image of batch
    if len(image.shape) == 4:
        image = image[0, :, :, :]
    plt.imshow(reverse_transforms(image))
