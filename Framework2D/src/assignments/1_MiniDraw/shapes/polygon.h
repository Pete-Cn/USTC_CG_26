#pragma once

#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "shape.h"

namespace USTC_CG
{
class Polygon : public Shape
{
   public:
    Polygon() = default;

    // Constructor to initialize a line with start and end coordinates
    Polygon(
        float start_point_x,
        float start_point_y,
        float end_point_x,
        float end_point_y,
        const unsigned char line_color[4],
        const unsigned char fill_color[4],
        float thickness)
        : temp_end_point_(ImVec2(end_point_x, end_point_y)),
          im_flag(ImDrawFlags_None),
          is_finished(false)
    {
        point_list_.push_back(ImVec2(start_point_x, start_point_y));
        set_color(line_color_, line_color);
        set_color(fill_color_, fill_color);
        line_thickness_ = thickness;
    }
    virtual ~Polygon() = default;

    // Overrides draw function to implement line-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the end point during
    // interaction
    void update(float x, float y) override;

    void add_control_point(float x, float y) override;

    void finish_draw() override;

   private:
    std::vector<ImVec2> point_list_;
    ImVec2 temp_end_point_;
    ImDrawFlags im_flag;
    bool is_finished;
};
}  // namespace USTC_CG
