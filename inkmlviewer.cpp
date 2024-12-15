#include "inkmlviewer.hpp"

#include <gtkmm-4.0/gtkmm.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>

#include "myxmlparser.hpp"

InkArea::InkArea(
    const std::string &ground_truth,
    const std::vector<std::vector<std::tuple<float, float>>> &strokes) {
    this->ground_truth = ground_truth;
    this->strokes = strokes;
    set_draw_func(sigc::mem_fun(*this, &InkArea::on_draw));
}

InkArea::~InkArea() {
    // Delete the temporary file if it exists
    std::remove(_tmp_svg);
}

// Convert RGB values to hex color code
std::string rgb_to_hex(float r, float g, float b) {
    int ri = static_cast<int>(r * 255);
    int gi = static_cast<int>(g * 255);
    int bi = static_cast<int>(b * 255);
    char hex_color[8];
    snprintf(hex_color, sizeof(hex_color), "#%02X%02X%02X", ri, gi, bi);
    return std::string(hex_color);
}

void InkArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr, int width,
                      int height) {
    // Determine the scaling and translation needed to fit the drawing
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    // First, find the bounding box of all strokes
    for (const auto &stroke : strokes) {
        for (const auto &[x, y] : stroke) {
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
    }

    // Calculate scaling and translation
    float scale_x = width / (max_x - min_x);
    float scale_y = height / (max_y - min_y);
    float scale =
        std::min(scale_x, scale_y) * 0.9;  // 90% of canvas to add some padding

    // Center the drawing (Add 0.0001 to avoid divide by zero errors)
    float translate_x =
        width / 2.0f - ((max_x - min_x + 0.0001) / 2.0f) * scale;
    float translate_y =
        height / 2.0f - ((max_y - min_y + 0.0001) / 2.0f) * scale;

    cr->save();
    cr->set_source_rgb(0.118, 0.118, 0.18);
    cr->paint();
    // Set up drawing properties
    cr->set_line_width(5.0);
    cr->set_line_cap(Cairo::Context::LineCap::ROUND);
    cr->set_line_join(Cairo::Context::LineJoin::ROUND);
    // Color of strokes and latex
    float r = 0.953, g = 0.545, b = 0.659;
    cr->set_source_rgb(r, g, b);

    // Loop through all strokes
    for (const auto &stroke : strokes) {
        // Move to the first point in the stroke
        auto [first_x, first_y] = stroke.front();
        cr->move_to(translate_x + (first_x - min_x) * scale,
                    translate_y + (first_y - min_y) * scale);
        // Loop through all points in stroke
        for (size_t i = 1; i < stroke.size(); ++i) {
            const auto [x, y] = stroke[i];
            cr->line_to(translate_x + (x - min_x) * scale,
                        translate_y + (y - min_y) * scale);
        }
        // Draw stroke
        cr->stroke();
    }
    cr->restore();

    // Convert LaTeX to SVG
    auto latex_to_svg = [](const std::string &latex) {
        // Run tex2svg and capture the SVG output
        FILE *pipe = popen(("tex2svg \"" + latex + "\"").c_str(), "r");
        if (!pipe) {
            std::cerr << "Error running tex2svg!" << std::endl;
            return std::string("");
        }

        std::stringstream buffer;
        char tmp[128];
        while (fgets(tmp, sizeof(tmp), pipe) != nullptr) {
            buffer << tmp;
        }
        fclose(pipe);

        return buffer.str();
    };

    std::string latex_svg = latex_to_svg(ground_truth);
    if (latex_svg.empty()) {
        return;
    }

    // Replace all instances of "currentColor" with the hex color
    std::string hex_color = rgb_to_hex(r, g, b);
    size_t pos = 0;
    while ((pos = latex_svg.find("currentColor", pos)) != std::string::npos) {
        latex_svg.replace(pos, 12, hex_color);  // "currentColor" length is 12
        pos += hex_color.length();  // Move past the newly inserted color
    }

    char tmp_svg[] = "/tmp/svgfile_XXXXXX";
    int fd = mkstemp(tmp_svg);
    if (fd == -1) {
        std::cerr << "Error creating temporary SVG file!" << std::endl;
        return;
    }
    close(fd);

    std::ofstream svg_file(tmp_svg);
    svg_file << latex_svg;
    svg_file.close();

    // Load the SVG into a Pixbuf
    Glib::RefPtr<Gdk::Pixbuf> latex_pixbuf;
    try {
        latex_pixbuf =
            Gdk::Pixbuf::create_from_file(tmp_svg, width / 4.0, height / 4.0);
    } catch (const Glib::Error &ex) {
        std::cerr << "Error loading SVG file: " << ex.what() << std::endl;
        return;
    }

    cr->save();
    cr->set_source_rgb(1, 0, 0);
    Gdk::Cairo::set_source_pixbuf(cr, latex_pixbuf, width * 3 / 8, 0);
    cr->paint();
    cr->restore();

    _tmp_svg = tmp_svg;
}

MyWindow::MyWindow(
    const std::string &ground_truth,
    const std::vector<std::vector<std::tuple<float, float>>> &strokes)
    : ink_area(ground_truth, strokes) {
    set_title("InkML Viewer");
    set_default_size(500, 250);
    set_child(ink_area);
}

MyApplication::MyApplication()
    : Gtk::Application("org.gtkmm.inkmlviewer",
                       Gio::Application::Flags::HANDLES_OPEN) {}

Glib::RefPtr<MyApplication> MyApplication::create() {
    return Glib::make_refptr_for_instance<MyApplication>(new MyApplication());
}

// Convert local paths from args into global ones
std::string resolve(const std::string &input_path) {
    try {
        // If path is already absolute, return as-is
        if (std::filesystem::path(input_path).is_absolute()) {
            return input_path;
        }

        // Get current working directory
        std::filesystem::path current_path = std::filesystem::current_path();

        // Resolve the relative path
        std::filesystem::path full_path =
            std::filesystem::absolute(current_path / input_path);

        return full_path.string();
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Path resolution error: " << e.what() << std::endl;
        return input_path;
    }
}

MyWindow *MyApplication::create_appwindow(const std::string &filepath) {
    std::string parsed_ground_truth;
    std::vector<std::vector<std::tuple<float, float>>> parsed_strokes;
    try {
        MyXMLParser parser;
        parser.set_substitute_entities(true);
        parser.parse_file(resolve(filepath));

        // Log parsed ground truth and stroke data
        parsed_ground_truth = parser.get_ground_truth();
        parsed_strokes = parser.get_strokes();
    } catch (const std::exception &e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return nullptr;
    }
    MyWindow *appwindow = new MyWindow(parsed_ground_truth, parsed_strokes);
    add_window(*appwindow);
    // Delete the window when it is hidden.
    appwindow->signal_hide().connect([appwindow]() { delete appwindow; });
    return appwindow;
}

void MyApplication::on_activate() {
    const auto data_dirs = Glib::get_system_data_dirs();

    std::string filepath;
    for (const auto &dir : data_dirs) {
        auto candidate =
            Glib::build_filename(dir, "inkmlviewer", "example.inkml");
        if (Glib::file_test(candidate, Glib::FileTest::EXISTS)) {
            filepath = candidate;
            break;
        }
    }
    if (filepath.empty()) {
        std::cerr << "example.inkml not found in system data directories."
                  << std::endl;
    }
    auto appwindow = create_appwindow(filepath);
    appwindow->present();
}

void MyApplication::on_open(const Gio::Application::type_vec_files &files,
                            const Glib::ustring & /* hint */) {
    MyWindow *appwindow = nullptr;
    auto windows = get_windows();
    if (windows.size() > 0) {
        appwindow = dynamic_cast<MyWindow *>(windows[0]);
    }
    if (!appwindow) {
        appwindow = create_appwindow(files[0]->get_path());
    }
    appwindow->present();
}
