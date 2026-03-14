#pragma once

#include <imgui.h>
namespace USTC_CG
{
class Shape
{
   public:
    // Draw Settings
    struct Config
    {
        // Offset to convert canvas position to screen position
        float bias[2] = { 0.f, 0.f };
        // Line color in RGBA format
        unsigned char line_color[4] = { 255, 0, 0, 255 };
        float line_thickness = 2.0f;
    };

   public:
    virtual ~Shape() = default;

    /**
     * Draws the shape on the screen.
     * This is a pure virtual function that must be implemented by all derived
     * classes.
     *
     * @param config The configuration settings for drawing, including line
     * color, thickness, and bias.
     *               - line_color defines the color of the shape's outline.
     *               - line_thickness determines how thick the outline will be.
     *               - bias is used to adjust the shape's position on the
     * screen.
     */
    virtual void draw(const Config& config) const = 0;
    /**
     * Updates the state of the shape.
     * This function allows for dynamic modification of the shape, in response
     * to user interactions like dragging.
     *
     * @param x, y Dragging point. e.g. end point of a line.
     */
    virtual void update(float x, float y) = 0;
    /**
     * Adds a control point to the shape.
     * This function is used to add control points to the shape, which can be
     * used to modify the shape's appearance.
     *
     * @param x, y Control point to be added. e.g. vertex of a polygon.
     */
    virtual void add_control_point(float x, float y) {}

    virtual void finish_draw() {}

    void set_color(ImU32 &color_, const unsigned char color[4]) {
        color_ = IM_COL32(color[0], color[1], color[2], color[3]);
    }
   protected:
   // shape的颜色和粗细，默认红色，2.0f
    ImU32 line_color_ = IM_COL32(255, 0, 0, 255);
    float line_thickness_ = 2.0f; 
    
    // 填充颜色设置，默认的alpha为0，即不填充任何颜色
    ImU32 fill_color_ = IM_COL32(0, 0, 0, 0);
};
}  // namespace USTC_CG