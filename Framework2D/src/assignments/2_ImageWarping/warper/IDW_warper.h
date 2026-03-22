// HW2_TODO: Implement the IDWWarper class
#pragma once

#include <Eigen/Dense>
#include <vector>

#include "imgui.h"
#include "type_define.h"
#include "warper.h"

namespace USTC_CG
{
class IDWWarper : public Warper
{
   public:
    IDWWarper(
        const std::vector<ImVec2>& start_points,
        const std::vector<ImVec2>& end_points,
        const double& mu = 2.0)
        : m_mu_(mu)
    {
        is_valid_ = (m_mu_ >= 1.0) &&
                    (start_points.size() == end_points.size()) &&
                    (start_points.size() >= 1);
        n_points_ = static_cast<int>(start_points.size());
        m_P.resize(n_points_);
        m_Q.resize(n_points_);
        m_T.resize(n_points_);

        for (size_t i = 0; i < start_points.size(); ++i)
        {
            m_P[i] = V2D(start_points[i].x, start_points[i].y);
        }
        for (size_t i = 0; i < end_points.size(); ++i)
        {
            m_Q[i] = V2D(end_points[i].x, end_points[i].y);
        }
    };

    ~IDWWarper() = default;

    std::pair<int, int> warp(const int& x, const int& y) override;

    bool get_transform() override;

   private:
    double sigma(const V2D& p1, const V2D& p2) const;

    std::vector<M2D> m_T;
    double m_mu_;
};
}  // namespace USTC_CG