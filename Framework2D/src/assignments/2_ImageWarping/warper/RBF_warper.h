// HW2_TODO: Implement the RBFWarper class
#pragma once

#include <Eigen/Dense>
#include <vector>

#include "imgui.h"
#include "type_define.h"
#include "warper.h"

namespace USTC_CG
{
class RBFWarper : public Warper
{
   public:
    RBFWarper(
        const std::vector<ImVec2>& start_points,
        const std::vector<ImVec2>& end_points)
    {
        is_valid_ = (start_points.size() == end_points.size()) &&
                    (start_points.size() >= 1);
        n_points_ = static_cast<int>(start_points.size());
        m_P.resize(n_points_);
        m_Q.resize(n_points_);

        for (size_t i = 0; i < start_points.size(); ++i)
        {
            m_P[i] = V2D(start_points[i].x, start_points[i].y);
        }
        for (size_t i = 0; i < end_points.size(); ++i)
        {
            m_Q[i] = V2D(end_points[i].x, end_points[i].y);
        }
    };

    ~RBFWarper() = default;

    std::pair<int, int> warp(const int& x, const int& y) override;

    bool get_transform() override;

   private:
    double g(const V2D& p1, const V2D& p2, double r_i) const;

    M2D A_;
    V2D b_;
    std::vector<V2D> alpha_;
    std::vector<double> r_;
};
}  // namespace USTC_CG