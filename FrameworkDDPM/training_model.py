from forward_noising import forward_diffusion_sample, T
from unet import SimpleUnet
from dataloader import load_transformed_dataset
import torch.nn.functional as F
import torch
from torch.optim import Adam
import logging
from tqdm import trange
import cv2 as cv

logging.basicConfig(level=logging.INFO)

def get_loss(model, x_0, t, device):
    # 得到添加噪声的图片、时间步 t 和噪声
    x_noisy, noise = forward_diffusion_sample(x_0, t, device)
    # 输入模型进行预测
    noise_pred = model(x_noisy, t)
    # 计算 loss
    return F.mse_loss(noise, noise_pred)


if __name__ == "__main__":
    model = SimpleUnet()
    # T 从 forward_noising 导入，而不是输入
    BATCH_SIZE = 1
    epochs = 5000

    dataloader = load_transformed_dataset(batch_size=BATCH_SIZE)

    device = "cuda" if torch.cuda.is_available() else "cpu"
    # device = "cpu"
    logging.info(f"Using device: {device}")
    model.to(device)
    optimizer = Adam(model.parameters(), lr=1e-4)

    for epoch in range(epochs):
        for batch_idx, (batch, _) in enumerate(dataloader):
            optimizer.zero_grad()

            # 随机采样时间步
            t = torch.randint(0, T, (batch.shape[0],), device=device).long()
            
            loss = get_loss(model, batch, t, device)
            loss.backward()
            optimizer.step()

            if batch_idx % 50 == 0:
                logging.info(f"Epoch {epoch} | Batch index {batch_idx:03d} Loss: {loss.item()}")

    torch.save(model.state_dict(), f"./ddpm_mse_epochs_{epochs}.pth")
