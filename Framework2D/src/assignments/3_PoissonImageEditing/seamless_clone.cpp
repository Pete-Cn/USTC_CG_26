#include "seamless_clone.h"
#include <algorithm>
#include <cmath>

namespace USTC_CG
{

SeamlessClone::SeamlessClone(
    std::shared_ptr<Image> src_img,
    std::shared_ptr<Image> dst_img,
    int src_x, int src_y,
    int offset_x, int offset_y,
    int W, int H)
    : src_img_(src_img),
      dst_img_(dst_img),
      src_x_(src_x), src_y_(src_y),
      offset_x_(offset_x), offset_y_(offset_y),
      W_(W), H_(H)
{
}

void SeamlessClone::set_offset(int offset_x, int offset_y)
{
    offset_x_ = offset_x;
    offset_y_ = offset_y;
}

void SeamlessClone::precompute()
{
    int N = W_ * H_;
    Eigen::SparseMatrix<double> A(N, N);
    std::vector<Eigen::Triplet<double>> triplet_list;
    triplet_list.reserve(N * 5);

    for (int y = 0; y < H_; ++y)
    {
        for (int x = 0; x < W_; ++x)
        {
            int idx = y * W_ + x;
            triplet_list.push_back(Eigen::Triplet<double>(idx, idx, 4.0));
            if (y > 0)    triplet_list.push_back(Eigen::Triplet<double>(idx, (y - 1) * W_ + x, -1.0));
            if (y < H_-1) triplet_list.push_back(Eigen::Triplet<double>(idx, (y + 1) * W_ + x, -1.0));
            if (x > 0)    triplet_list.push_back(Eigen::Triplet<double>(idx, y * W_ + (x - 1), -1.0));
            if (x < W_-1) triplet_list.push_back(Eigen::Triplet<double>(idx, y * W_ + (x + 1), -1.0));
        }
    }

    A.setFromTriplets(triplet_list.begin(), triplet_list.end());
    solver_.compute(A);
}

std::shared_ptr<Image> SeamlessClone::solve()
{
    auto result = std::make_shared<Image>(*dst_img_);

    int dst_w = dst_img_->width();
    int dst_h = dst_img_->height();
    int src_w = src_img_->width();
    int src_h = src_img_->height();

    int N = W_ * H_;

    auto get_src = [&](int sx, int sy, int c) -> double
    {
        sx = std::clamp(sx, 0, src_w - 1);
        sy = std::clamp(sy, 0, src_h - 1);
        return static_cast<double>(src_img_->get_pixel(sx, sy)[c]);
    };

    auto get_dst = [&](int dx, int dy, int c) -> double
    {
        dx = std::clamp(dx, 0, dst_w - 1);
        dy = std::clamp(dy, 0, dst_h - 1);
        return static_cast<double>(dst_img_->get_pixel(dx, dy)[c]);
    };

    for (int c = 0; c < 3; ++c)
    {
        Eigen::VectorXd B(N);

        for (int y = 0; y < H_; ++y)
        {
            for (int x = 0; x < W_; ++x)
            {
                int idx = y * W_ + x;

                int sx = src_x_ + x;
                int sy = src_y_ + y;
                // Guidance field: Laplacian of source image g
                B(idx) = 4.0 * get_src(sx, sy, c)
                       - get_src(sx - 1, sy, c)
                       - get_src(sx + 1, sy, c)
                       - get_src(sx, sy - 1, c)
                       - get_src(sx, sy + 1, c);

                // offset_x_/offset_y_ is the top-left of the region in dst image
                int dst_x = offset_x_ + x;
                int dst_y = offset_y_ + y;

                // Dirichlet boundary: add f*(neighbor) for border pixels
                if (x == 0)    B(idx) += get_dst(dst_x - 1, dst_y, c);
                if (x == W_-1) B(idx) += get_dst(dst_x + 1, dst_y, c);
                if (y == 0)    B(idx) += get_dst(dst_x, dst_y - 1, c);
                if (y == H_-1) B(idx) += get_dst(dst_x, dst_y + 1, c);
            }
        }

        Eigen::VectorXd r = solver_.solve(B);

        for (int y = 0; y < H_; ++y)
        {
            for (int x = 0; x < W_; ++x)
            {
                int dst_x = offset_x_ + x;
                int dst_y = offset_y_ + y;
                if (dst_x < 0 || dst_x >= dst_w || dst_y < 0 || dst_y >= dst_h)
                    continue;
                auto pixel = result->get_pixel(dst_x, dst_y);
                pixel[c] = static_cast<unsigned char>(
                    std::clamp(static_cast<int>(std::round(r(y * W_ + x))), 0, 255));
                result->set_pixel(dst_x, dst_y, pixel);
            }
        }
    }

    return result;
}

}  // namespace USTC_CG
