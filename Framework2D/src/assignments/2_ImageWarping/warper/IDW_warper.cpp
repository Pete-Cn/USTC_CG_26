#include "IDW_warper.h"

#include "type_define.h"
namespace USTC_CG
{

std::pair<int, int> IDWWarper::warp(const int& x, const int& y)
{
    if (!is_valid_)
    {
        return std::make_pair(x, y);
    }
    V2D p_(x, y);

    // 只有一个点：平移变换
    if (n_points_ == 1)
    {
        return std::make_pair(
            static_cast<int>(p_.x() + m_Q[0].x() - m_P[0].x()),
            static_cast<int>(p_.y() + m_Q[0].y() - m_P[0].y()));
    }

    for (int i = 0; i < n_points_; ++i)
    {
        if ((p_ - m_P[i]).norm() < EPS)
        {
            return std::make_pair(
                static_cast<int>(m_Q[i].x()), static_cast<int>(m_Q[i].y()));
        }
    }

    double sigma_sum_ = 0.0;
    std::vector<double> sigma_array_;
    sigma_array_.resize(n_points_);

    // 计算输入点与每个控制点的 sigma，为后续权重计算做准备
    for (int i = 0; i < n_points_; ++i)
    {
        sigma_array_[i] = sigma(p_, m_P[i]);
        sigma_sum_ += sigma_array_[i];
    }

    V2D q_ = V2D::Zero();
    for (int i = 0; i < n_points_; ++i)
    {
        double w_i = sigma_array_[i] / sigma_sum_;
        V2D f_i = m_Q[i] + m_T[i] * (p_ - m_P[i]);
        q_ += w_i * f_i;
    }

    return std::make_pair(
        static_cast<int>(q_.x()), static_cast<int>(q_.y()));
}

bool IDWWarper::get_transform()
{
    if (!is_valid_) return false;
    // 只有一个点：平移
    if (n_points_ == 1)
    {
        m_T[0] = M2D::Identity();
        return true;
    }

    // 两个点：平移 + 缩放 + 旋转
    // 只需要记录相同的 T，然后跟据离控制点的距离做一个加权变换即可
    if (n_points_ == 2)
    {
        V2D dp = m_P[0] - m_P[1];
        V2D dq = m_Q[0] - m_Q[1];
        double alpha = dq.norm() / (dp.norm() + EPS);
        double angle = std::atan2(dq.y(), dq.x()) - std::atan2(dp.y(), dp.x());
        M2D R;
        R << std::cos(angle), -std::sin(angle), std::sin(angle),
            std::cos(angle);
        m_T[0] = alpha * R;
        m_T[1] = alpha * R;
        return true;
    }
    int n = static_cast<int>(m_P.size());

    for (int i = 0; i < n; ++i)
    {
        M2D A = M2D::Zero();
        M2D B = M2D::Zero();

        for (int j = 0; j < n; ++j)
        {
            if (i == j)
                continue;

            double sigma_ij = sigma(m_P[i], m_P[j]);

            V2D dp_ = m_P[j] - m_P[i];
            V2D dq_ = m_Q[j] - m_Q[i];

            // 跟据公式得出
            A += sigma_ij * dp_ * dp_.transpose();
            B += sigma_ij * dq_ * dp_.transpose();
        }

        // 添加正则项，避免求逆失败
        A += EPS * M2D::Identity();

        m_T[i] = B * A.inverse();
    }
    return true;
}

double IDWWarper::sigma(const V2D& p1, const V2D& p2) const
{
    double d_ = (p1 - p2).norm();
    if (d_ < EPS)
        return INF;
    return 1.0 / std::pow(d_, m_mu_);
}

}  // namespace USTC_CG