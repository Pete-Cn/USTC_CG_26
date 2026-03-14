#include "ellipse.h"

#include <imgui.h>
#include <math.h>

namespace USTC_CG
{
// Draw the line using ImGui
void Ellipse::draw(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 center = ImVec2(
        config.bias[0] + (start_point_x_ + end_point_x_) / 2.0,
        config.bias[1] + (start_point_y_ + end_point_y_) / 2.0);
    ImVec2 radius = ImVec2(
        fabs(end_point_x_ - start_point_x_) / 2.0,
        fabs(end_point_y_ - start_point_y_) / 2.0);

    draw_list->AddEllipseFilled(center, radius, fill_color_, 0.0f, 0);

    // Draw border
    draw_list->AddEllipse(
        center, radius, line_color_, 0.0f, 0, line_thickness_);
}

void Ellipse::update(float x, float y)
{
    end_point_x_ = x;
    end_point_y_ = y;
}
}  // namespace USTC_CG