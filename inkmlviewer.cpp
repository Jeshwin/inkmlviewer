#include <gtkmm-4.0/gtkmm.h>
#include "inkmlviewer.h"
#include <iostream>
#include <limits>
#include <cmath>

InkArea::InkArea(const std::string ground_truth,
                 const std::vector<std::vector<std::tuple<float, float>>>& strokes){
    this->ground_truth = ground_truth;
    this->strokes = strokes;
    set_draw_func(sigc::mem_fun(*this, &InkArea::on_draw));
}

InkArea::~InkArea() {}

void InkArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    // Determine the scaling and translation needed to fit the drawing
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    // First, find the bounding box of all strokes
    for (const auto& stroke : strokes) {
        for (const auto& [x, y] : stroke) {
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
    }
    std::cout << "Min X: " << min_x << '\n';
    std::cout << "Max X: " << max_x << '\n';
    std::cout << "Min Y: " << min_y << '\n';
    std::cout << "Max Y: " << max_y << '\n';    

    // Calculate scaling and translation
    float scale_x = width / (max_x - min_x);
    float scale_y = height / (max_y - min_y);
    float scale = std::min(scale_x, scale_y) * 0.9; // 90% of canvas to add some padding

    // Center the drawing
    float translate_x = width / 2.0f - ((max_x - min_x) / 2.0f) * scale;
    float translate_y = height / 2.0f - ((max_y - min_y) / 2.0f) * scale;
    std::cout << "Width: " << width << '\n';
    std::cout << "Height: " << height << '\n';
    std::cout << "Unscaled Width: " << max_x - min_x << '\n';
    std::cout << "Unscaled Height: " << max_y - min_y << '\n';
    std::cout << "Scale: " << scale << '\n';
    std::cout << "translate_x: " << translate_x << '\n';
    std::cout << "translate_y: " << translate_y << '\n';

    cr->save();
    cr->set_source_rgb(0, 0, 0);
    cr->paint();
    // Set up drawing properties
    cr->set_line_width(5.0);
    cr->set_line_cap(Cairo::Context::LineCap::ROUND);
    cr->set_line_join(Cairo::Context::LineJoin::ROUND);
    cr->set_source_rgb(1, 1, 1); 

    // Loop through all strokes
    for (const auto &stroke : strokes) {
        // Move to the first point in the stroke
        auto [first_x, first_y] = stroke.front();
        cr->move_to(
            translate_x + (first_x - min_x) * scale,
            translate_y + (first_y - min_y) * scale
        );
        // Loop through all points in stroke
        for (size_t i = 1; i < stroke.size(); ++i) {
            const auto [x, y] = stroke[i];
            cr->line_to(
                translate_x + (x - min_x) * scale,
                translate_y + (y - min_y) * scale
            );
        }
        // Draw stroke
        cr->stroke();
    }
    cr->restore();
}

MyWindow::MyWindow(const std::string ground_truth,
                   const std::vector<std::vector<std::tuple<float, float>>>& strokes)
: ink_area(ground_truth, strokes) {
	set_title("InkML Viewer");
	set_default_size(200, 200);
    set_child(ink_area);
}
