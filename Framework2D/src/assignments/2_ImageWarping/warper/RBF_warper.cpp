#include "RBF_warper.h"

#include <cmath>
#include <iostream>

namespace USTC_CG
{

std::pair<int, int> RBFWarper::warp(const int& x, const int& y)
{
    if (!is_valid_)
    {
        return std::make_pair(x, y);
    }

    V2D p_(x, y);

    V2D q_ = A_ * p_ + b_;
    
    // 计算 R(p)
    for (int i = 0; i < n_points_; ++i)
    {
        q_ += alpha_[i] * g(p_, m_P[i], r_[i]);
    }

    return std::make_pair(
        static_cast<int>(q_.x()), static_cast<int>(q_.y()));
}

double RBFWarper::g(const V2D& p1, const V2D& p2, double r_i) const
{
    return std::sqrt((p1 - p2).squaredNorm() + r_i * r_i);
}

bool RBFWarper::get_transform()
{
    if (!is_valid_) return false;
    // 计算 r_i
    r_.resize(n_points_);
    for (int i = 0; i < n_points_; ++i)
    {
        double min_d = INF;
        for (int j = 0; j < n_points_; ++j)
        {
            if (i == j)
                continue;
            double d = (m_P[i] - m_P[j]).norm();
            if (d < min_d)
                min_d = d;
        }
        if (n_points_ == 1)
        {
            min_d = 0.0;
        }
        r_[i] = min_d;
    }

    A_ = M2D::Identity();
    b_ = V2D::Zero();
    alpha_.resize(n_points_, V2D::Zero());

    // 计算 A 和 b
    if (n_points_ == 1)
    {
        A_ = M2D::Identity();
        b_ = m_Q[0] - m_P[0];
        return true;
    }
    if (n_points_ == 2)
    {
        V2D dp = m_P[0] - m_P[1];
        V2D dq = m_Q[0] - m_Q[1];
        double scale = dq.norm() / (dp.norm() + EPS);
        double angle = std::atan2(dq.y(), dq.x()) - std::atan2(dp.y(), dp.x());
        M2D R;
        R << std::cos(angle), -std::sin(angle), std::sin(angle),
            std::cos(angle);
        A_ = scale * R;
        b_ = m_Q[0] - A_ * m_P[0];
        return true;
    }

    MatrixXd P_mat(n_points_, 3);
    for (int i = 0; i < n_points_; ++i)
    {
        P_mat(i, 0) = m_P[i].x();
        P_mat(i, 1) = m_P[i].y();
        P_mat(i, 2) = 1.0;
    }

    MatrixXd Q_mat(n_points_, 2);
    for (int i = 0; i < n_points_; ++i)
    {
        Q_mat(i, 0) = m_Q[i].x();
        Q_mat(i, 1) = m_Q[i].y();
    }

    // 最小二乘问题： P_mat * C_mat = Q_mat
    // C_mat = [A, b]^T
    // => C_mat = (P^T P)^{-1} P^T Q
    MatrixXd C_mat = P_mat.colPivHouseholderQr().solve(Q_mat);  // 3×2

    A_(0, 0) = C_mat(0, 0);
    A_(0, 1) = C_mat(1, 0);
    A_(1, 0) = C_mat(0, 1);
    A_(1, 1) = C_mat(1, 1);
    b_(0) = C_mat(2, 0);
    b_(1) = C_mat(2, 1);

    // 跟据得出的 A, b, r，计算 alpha
    // 即 alpha 满足：
    // A * P_i + b + sum_{j=1}^{n} alpha_j * g(P_i, P_j) = Q_i
    // => R_i = Q_i - (A * P_i + b) = sum_{j=1}^{n} alpha_j * g(P_i, P_j)
    // 这同样是个线性方程组，可以写成：
    // G * Alpha_mat = R

    MatrixXd G(n_points_, n_points_);
    MatrixXd R(n_points_, 2);

    for (int i = 0; i < n_points_; ++i)
    {
        // R_i = Q_i - (A*P_i + b)
        V2D R_i = m_Q[i] - (A_ * m_P[i] + b_);
        R(i, 0) = R_i.x();
        R(i, 1) = R_i.y();

        // 构造 G
        for (int j = 0; j < n_points_; ++j)
        {
            G(i, j) = g(m_P[i], m_P[j], r_[j]);
        }
    }

    // 求解 G * Alpha_mat = R
    MatrixXd Alpha_mat = G.colPivHouseholderQr().solve(R);

    for (int i = 0; i < n_points_; ++i)
    {
        alpha_[i] = V2D(Alpha_mat(i, 0), Alpha_mat(i, 1));
    }
    return true;
}

}  // namespace USTC_CG
