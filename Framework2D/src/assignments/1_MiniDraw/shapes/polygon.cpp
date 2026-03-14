#include "polygon.h"

#include <imgui.h>

#include <vector>

namespace USTC_CG
{
// Draw the line using ImGui
void Polygon::draw(const Config& config) const
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
    if (!is_finished)
    {
#ifdef IMGUI_DEFINE_MATH_OPERATORS
        bias_point_list_.push_back(bias_imvec2 + temp_end_point_);
#else
        bias_point_list_.push_back(ImVec2(
            bias_imvec2.x + temp_end_point_.x,
            bias_imvec2.y + temp_end_point_.y));
#endif
    }

    if (is_finished)
    {
        draw_list->AddConvexPolyFilled(
            bias_point_list_.data(),
            static_cast<int>(bias_point_list_.size()),
            fill_color_);
    }
    // Draw border
    draw_list->AddPolyline(
        bias_point_list_.data(),
        static_cast<int>(bias_point_list_.size()),
        line_color_,
        im_flag,
        line_thickness_);
}

void Polygon::update(float x, float y)
{
    if (im_flag == ImDrawFlags_None)
    {
        temp_end_point_ = ImVec2(x, y);
    }
}

void Polygon::finish_draw()
{
    if (point_list_.size() > 2)  // 如果点数超过2个，则可以组成封闭图形
    {
        im_flag = ImDrawFlags_Closed;
    }  // 否则，保持为ImDrawFlags_None，这样做可以满足退化成直线的情况
    is_finished = true;
}

void Polygon::add_control_point(float x, float y)
{
    point_list_.push_back(ImVec2(x, y));
}
}  // namespace USTC_CG