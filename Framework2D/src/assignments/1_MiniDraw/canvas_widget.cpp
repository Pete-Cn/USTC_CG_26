#include "canvas_widget.h"

#include <cmath>
#include <iostream>

#include "imgui.h"
#include "shapes/ellipse.h"
#include "shapes/freehand.h"
#include "shapes/line.h"
#include "shapes/polygon.h"
#include "shapes/rect.h"

namespace USTC_CG
{
// 每次调用draw
void Canvas::draw()
{
    draw_background();
    // HW1_TODO: more interaction events
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        mouse_click_event();
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        mouse_right_click_event();
    mouse_move_event();
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        mouse_release_event();

    draw_shapes();
}

// 设置画布大小
void Canvas::set_attributes(const ImVec2& min, const ImVec2& size)
{
    canvas_min_ = min;
    canvas_size_ = size;
    canvas_minimal_size_ = size;
    canvas_max_ =
        ImVec2(canvas_min_.x + canvas_size_.x, canvas_min_.y + canvas_size_.y);
}

void Canvas::show_background(bool flag)
{
    show_background_ = flag;
}

// 点击button设置当前绘制的图形类型
void Canvas::set_default()
{
    draw_status_ = false;
    shape_type_ = kDefault;
}

void Canvas::set_line()
{
    draw_status_ = false;
    shape_type_ = kLine;
}

void Canvas::set_rect()
{
    draw_status_ = false;
    shape_type_ = kRect;
}

// HW1_TODO: more shape types, implements
void Canvas::set_ellipse()
{
    draw_status_ = false;
    shape_type_ = kEllipse;
}

void Canvas::set_polygon()
{
    draw_status_ = false;
    shape_type_ = kPolygon;
}

void Canvas::set_freehand()
{
    draw_status_ = false;
    shape_type_ = kFreehand;
}

void Canvas::clear_shape_list()
{
    shape_list_.clear();
}

void Canvas::draw_background()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (show_background_)
    {
        // Draw background recrangle
        draw_list->AddRectFilled(canvas_min_, canvas_max_, background_color_);
        // Draw background border
        draw_list->AddRect(canvas_min_, canvas_max_, border_color_);
    }
    /// Invisible button over the canvas to capture mouse interactions.
    ImGui::SetCursorScreenPos(canvas_min_);
    ImGui::InvisibleButton(
        label_.c_str(), canvas_size_, ImGuiButtonFlags_MouseButtonLeft);
    // Record the current status of the invisible button
    // 如果鼠标在画布的invisible button上（在画布上）
    is_hovered_ = ImGui::IsItemHovered();
    // 是否按下鼠标
    is_active_ = ImGui::IsItemActive();
}

// canvas会绘制shape-list的所有shape
void Canvas::draw_shapes()
{
    // Shape::Config s = { .bias = { canvas_min_.x, canvas_min_.y } };
    Shape::Config s = {
        .bias = { canvas_min_.x, canvas_min_.y },
        .line_color = { static_cast<unsigned char>(current_line_color_.x * 255),
                        static_cast<unsigned char>(current_line_color_.y * 255),
                        static_cast<unsigned char>(current_line_color_.z * 255),
                        static_cast<unsigned char>(
                            current_line_color_.w * 255) },
        .line_thickness = current_line_thickness_
    };

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // ClipRect can hide the drawing content outside of the rectangular area
    draw_list->PushClipRect(canvas_min_, canvas_max_, true);
    for (const auto& shape : shape_list_)
    {
        shape->draw(s);
    }
    // 额外绘制当前正在绘制的shape
    if (draw_status_ && current_shape_)
    {
        current_shape_->draw(s);
    }
    draw_list->PopClipRect();
}

// 如果鼠标在画布上并且刚按下
void Canvas::mouse_click_event()
{
    // HW1_TODO: Drawing rule for more primitives
    if (!draw_status_)
    {
        draw_status_ = true;
        start_point_ = end_point_ = mouse_pos_in_canvas();

        // 每次在初始化一个shape的时候需要额外传入一个颜色和粗细
        unsigned char line_color[4] = {
            static_cast<unsigned char>(current_line_color_.x * 255),
            static_cast<unsigned char>(current_line_color_.y * 255),
            static_cast<unsigned char>(current_line_color_.z * 255),
            static_cast<unsigned char>(current_line_color_.w * 255)
        };
        unsigned char fill_color[4] = {
            static_cast<unsigned char>(current_fill_color_.x * 255),
            static_cast<unsigned char>(current_fill_color_.y * 255),
            static_cast<unsigned char>(current_fill_color_.z * 255),
            static_cast<unsigned char>(current_fill_color_.w * 255)
        };
        switch (shape_type_)
        {
            case USTC_CG::Canvas::kDefault:
            {
                break;
            }
            case USTC_CG::Canvas::kLine:
            {
                current_shape_ = std::make_shared<Line>(
                    start_point_.x,
                    start_point_.y,
                    end_point_.x,
                    end_point_.y,
                    line_color,
                    current_line_thickness_);
                break;
            }
            case USTC_CG::Canvas::kRect:
            {
                current_shape_ = std::make_shared<Rect>(
                    start_point_.x,
                    start_point_.y,
                    end_point_.x,
                    end_point_.y,
                    line_color,
                    fill_color,
                    current_line_thickness_);
                break;
            }
            // HW1_TODO: case USTC_CG::Canvas::kEllipse:
            case USTC_CG::Canvas::kEllipse:
            {
                current_shape_ = std::make_shared<Ellipse>(
                    start_point_.x,
                    start_point_.y,
                    end_point_.x,
                    end_point_.y,
                    line_color,
                    fill_color,
                    current_line_thickness_);
                break;
            }
            case USTC_CG::Canvas::kPolygon:
            {
                current_shape_ = std::make_shared<Polygon>(
                    start_point_.x,
                    start_point_.y,
                    end_point_.x,
                    end_point_.y,
                    line_color,
                    fill_color,
                    current_line_thickness_);
                break;
            }
            case USTC_CG::Canvas::kFreehand:
            {
                current_shape_ = std::make_shared<Freehand>(
                    start_point_.x,
                    start_point_.y,
                    line_color,
                    current_line_thickness_);
                break;
            }
            default: break;
        }
    }
    else
    {
        if (current_shape_)
        {
            if (shape_type_ == Canvas::kPolygon)
            {
                current_shape_->add_control_point(end_point_.x, end_point_.y);
            }
            else
            {
                draw_status_ = false;
                shape_list_.push_back(current_shape_);
                current_shape_.reset();
            }
        }
        else
        {
            draw_status_ = false;
        }
    }
}

void Canvas::mouse_right_click_event()
{
    if (draw_status_)
    {
        if (current_shape_)
        {
            if (shape_type_ == Canvas::kPolygon)
            {
                current_shape_->finish_draw();
                draw_status_ = false;
                shape_list_.push_back(current_shape_);
                current_shape_.reset();
            }
        }
    }
}

// 如果正在绘制，endpoint=当前鼠标位置
void Canvas::mouse_move_event()
{
    // HW1_TODO: Drawing rule for more primitives
    if (draw_status_)
    {
        end_point_ = mouse_pos_in_canvas();
        // 如果有正在绘制的shape则更新
        if (current_shape_)
        {
            current_shape_->update(end_point_.x, end_point_.y);
        }
    }
}

// 如果松开鼠标左键
void Canvas::mouse_release_event()
{
    // HW1_TODO: Drawing rule for more primitives
    if (draw_status_)
    {
        if (current_shape_)
        {
            if (shape_type_ == Canvas::kFreehand)
            {
                draw_status_ = false;
                shape_list_.push_back(current_shape_);
                current_shape_.reset();
            }
        }
    }
}

ImVec2 Canvas::mouse_pos_in_canvas() const
{
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 mouse_pos_in_canvas(
        io.MousePos.x - canvas_min_.x, io.MousePos.y - canvas_min_.y);
    return mouse_pos_in_canvas;
}
}  // namespace USTC_CG