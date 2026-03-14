#pragma once

#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "shape.h"

namespace USTC_CG
{
class Freehand : public Shape
{
   public:
    Freehand() = default;

    // Constructor to initialize a line with start and end coordinates
    Freehand(
        float start_point_x,
        float start_point_y,
        const unsigned char line_color[4],
        float thickness)
    {
        point_list_.push_back(ImVec2(start_point_x, start_point_y));
        set_color(line_color_, line_color);
        line_thickness_ = thickness;
    }
    virtual ~Freehand() = default;

    // Overrides draw function to implement line-specific drawing logic
    void draw(const Config& config) const override;

    // Overrides Shape's update function to adjust the end point during
    // interaction
    void update(float x, float y) override;

   private:
    std::vector<ImVec2> point_list_;
};
}  // namespace USTC_CG
