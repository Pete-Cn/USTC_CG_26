#include "freehand.h"

#include <imgui.h>

#include <vector>

namespace USTC_CG
{
// Draw the line using ImGui
void Freehand::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 bias_imvec2 = ImVec2(config.bias[0], config.bias[1]);
    std::vector<ImVec2> bias_point_list_;
    bias_point_list_.clear();
    bias_point_list_.reserve(point_list_.size() + 1);
    for (const auto& pt_ : point_list_)
    {
#ifdef IMGUI_DEFINE_MATH_OPERATORS
        bias_point_list_.push_back(bias_imvec2 + pt_);
#else
        bias_point_list_.push_back(
            ImVec2(bias_imvec2.x + pt_.x, bias_imvec2.y + pt_.y));
#endif
    }
    draw_list->AddPolyline(
        bias_point_list_.data(),
        static_cast<int>(bias_point_list_.size()),
        line_color_,
        ImDrawFlags_None,
        line_thickness_);
}

void Freehand::update(float x, float y)
{
    ImVec2 p_curr_ = ImVec2(x, y);
    ImVec2 diff_;
#ifdef IMGUI_DEFINE_MATH_OPERATORS
    diff_ = p_curr_ - point_list_.back();
#else
    diff_ = ImVec2(
        p_curr_.x - point_list_.back().x, p_curr_.y - point_list_.back().y);
#endif
    if (diff_.x * diff_.x + diff_.y * diff_.y > 0.01)   // 只有当相邻点的距离大于一定程度的时候才会添加新点
    {
        point_list_.push_back(p_curr_);
    }
}
}  // namespace USTC_CG