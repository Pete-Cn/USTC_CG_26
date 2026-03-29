import torch
from forward_noising import (
    get_index_from_list,
    sqrt_one_minus_alphas_cumprod,
    betas,
    posterior_variance,
    sqrt_recip_alphas,
    forward_diffusion_sample,
    # 从前向传播处导入 T
    T,
)
import matplotlib.pyplot as plt
from dataloader import show_tensor_image
from unet import SimpleUnet
import numpy as np
import cv2 as cv


@torch.no_grad()
def sample_timestep(model, x, t):
    betas_t = get_index_from_list(betas, t, x.shape)
    sqrt_one_minus_alphas_cumprod_t = get_index_from_list(sqrt_one_minus_alphas_cumprod, t, x.shape)
    sqrt_recip_alphas_t = get_index_from_list(sqrt_recip_alphas, t, x.shape)

    # x_{t-1} = sqrt_recip_alpha_t * (x_t - beta_t / sqrt_one_minus_alpha_cumprod_t * predicted_noise) + sigma_t * z
    model_mean = sqrt_recip_alphas_t * (x - betas_t * model(x, t) / sqrt_one_minus_alphas_cumprod_t)

    posterior_variance_t = get_index_from_list(posterior_variance, t, x.shape)

    if t == 0:
        return model_mean
    else:
        noise = torch.randn_like(x)
        return model_mean + torch.sqrt(posterior_variance_t) * noise


@torch.no_grad()
def sample_plot_image(model, device, img_size, T):
    img = torch.randn((1, 3, img_size, img_size), device=device)
    num_images = 16
    step_size = T // num_images

    plt.figure(figsize=(18, 18))
    plot_idx = 1

    for i in range(0, T)[::-1]:
        t = torch.full((1,), i, device=device, dtype=torch.long)
        img = sample_timestep(model, img, t)
        img = torch.clamp(img, -1.0, 1.0)

        if i % step_size == 0:
            plt.subplot(4, 4, plot_idx)
            show_tensor_image(img.detach().cpu())
            plt.axis('off') 
            plot_idx += 1

    plt.savefig("generated_image.png", dpi=1200, bbox_inches="tight")
    plt.show()
    print("图片已保存到 generated_image.png")

    final_img = img.squeeze(0).detach().cpu()       # (3, H, W)
    final_img = (final_img + 1) / 2                 # [-1, 1] → [0, 1]
    final_img = final_img.clamp(0, 1)
    save_image(final_img, "final_generated_image.png")
    print("最终图片已保存到 final_generated_image.png")


def test_image_generation():
    device = "cuda" if torch.cuda.is_available() else "cpu"

    model = SimpleUnet()
    model.to(device)

    # 自动查找最新的模型文件
    import glob
    import os

    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_files = glob.glob(os.path.join(script_dir, "ddpm_*.pth"))
    if not model_files:
        print("错误：未找到训练好的模型文件 (ddpm_*.pth)，请先运行 training_model.py 进行训练。")
        return

    model_path = max(model_files, key=os.path.getmtime)
    print(f"加载模型: {model_path}")
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()

    img_size = 256
    sample_plot_image(model, device, img_size, T)


@torch.no_grad()
def inpaint(model, device, img, mask, t_max=50):
    """
    使用 RePaint 方法进行图像补全。
    img:  原始图像张量 (1, 3, H, W)，值域 [-1, 1]
    mask: 掩码张量 (1, 1, H, W) 或 (1, 3, H, W)，1 表示已知区域，0 表示需要补全的区域
    t_max: 最大时间步（控制补全强度）
    """
    img = img.to(device)
    mask = mask.to(device)

    # 从高斯噪声初始化待补全区域
    x = torch.randn_like(img, device=device)

    for i in range(t_max - 1, -1, -1):
        t = torch.full((1,), i, device=device, dtype=torch.long)

        # 对已知区域进行前向加噪到时间步 t
        noised_img, _ = forward_diffusion_sample(img, t, device)

        # 对未知区域进行单步去噪
        x = sample_timestep(model, x, t)
        x = torch.clamp(x, -1.0, 1.0)

        # RePaint 核心：已知区域用加噪后的原图，未知区域用去噪结果
        x = mask * noised_img + (1 - mask) * x

    return x


def test_image_inpainting():
    """
    加载训练好的模型和测试图像，调用 inpaint 进行图像补全。
    """
    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Using device: {device}")

    model = SimpleUnet()
    model.to(device)

    # 加载训练好的模型权重
    import glob
    import os

    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_files = glob.glob(os.path.join(script_dir, "ddpm_*.pth"))
    if not model_files:
        print("错误：未找到训练好的模型文件 (ddpm_*.pth)，请先运行 training_model.py 进行训练。")
        return

    model_path = max(model_files, key=os.path.getmtime)
    print(f"加载模型: {model_path}")
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()

    img_size = 256

    # 加载测试图像
    import tkinter as tk
    from tkinter import filedialog

    root = tk.Tk()
    root.withdraw()
    root.attributes("-topmost", True)

    print("请选择需要补全的图像文件...")
    img_path = filedialog.askopenfilename(
        title="请选择需要补全的图像",
        filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp")],
        parent=root,
    )
    if not img_path:
        root.destroy()
        print("未选择图像，退出。")
        return

    print("请选择掩码图像文件（白色=已知区域，黑色=待补全区域）...")
    mask_path = filedialog.askopenfilename(
        title="请选择掩码图像（白色=保留，黑色=补全）",
        filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp")],
        parent=root,
    )
    if not mask_path:
        root.destroy()
        print("未选择掩码图像，退出。")
        return

    root.destroy()

    # 读取并预处理图像
    img_cv = cv.imread(img_path)
    img_cv = cv.cvtColor(img_cv, cv.COLOR_BGR2RGB)
    img_cv = cv.resize(img_cv, (img_size, img_size))
    img_tensor = torch.from_numpy(img_cv).float().permute(2, 0, 1) / 255.0
    img_tensor = img_tensor * 2 - 1  # 归一化到 [-1, 1]
    img_tensor = img_tensor.unsqueeze(0).to(device)

    # 读取并预处理掩码
    mask_cv = cv.imread(mask_path, cv.IMREAD_GRAYSCALE)
    mask_cv = cv.resize(mask_cv, (img_size, img_size))
    mask_tensor = torch.from_numpy(mask_cv).float() / 255.0
    mask_tensor = (mask_tensor > 0.5).float()  # 二值化
    mask_tensor = mask_tensor.unsqueeze(0).unsqueeze(0).to(device)  # (1, 1, H, W)

    # 执行补全
    print("正在进行图像补全...")
    result = inpaint(model, device, img_tensor, mask_tensor, t_max=50)

    # 可视化结果
    plt.figure(figsize=(15, 5))

    plt.subplot(1, 3, 1)
    plt.title("Original Image")
    show_tensor_image(img_tensor.detach().cpu())

    plt.subplot(1, 3, 2)
    plt.title("Mask (white=keep)")
    plt.imshow(mask_cv, cmap="gray")

    plt.subplot(1, 3, 3)
    plt.title("Inpainted Result")
    show_tensor_image(result.detach().cpu())

    plt.savefig("inpainted_result.png", dpi=150, bbox_inches="tight")
    plt.show()
    print("补全结果已保存为 inpainted_result.png")


if __name__ == "__main__":
    test_image_generation()
    test_image_inpainting()