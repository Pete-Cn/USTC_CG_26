#pragma once

#include <memory>
#include <Eigen/Sparse>
#include "common/image.h"

namespace USTC_CG
{

class SeamlessClone
{
public:
    // src_img: source image g
    // dst_img: background/target image f*
    // src_x, src_y: top-left of the selected rect in source image
    // offset_x, offset_y: top-left of the region in target image (= mouse_x, mouse_y)
    // W, H: width and height of the rectangular region
    SeamlessClone(
        std::shared_ptr<Image> src_img,
        std::shared_ptr<Image> dst_img,
        int src_x, int src_y,
        int offset_x, int offset_y,
        int W, int H);

    void set_offset(int offset_x, int offset_y);
    void precompute();
    std::shared_ptr<Image> solve();

private:
    std::shared_ptr<Image> src_img_;
    std::shared_ptr<Image> dst_img_;
    int src_x_, src_y_;
    int offset_x_, offset_y_;
    int W_, H_;

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver_;
};

}  // namespace USTC_CG
