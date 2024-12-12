#include <gtkmm-4.0/gtkmm.h>
#include "inkmlviewer.h"
#include "myxmlparser.h"
#include <iostream>
#include <filesystem>
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

    // Calculate scaling and translation
    float scale_x = width / (max_x - min_x);
    float scale_y = height / (max_y - min_y);
    float scale = std::min(scale_x, scale_y) * 0.9; // 90% of canvas to add some padding

    // Center the drawing
    float translate_x = width / 2.0f - ((max_x - min_x) / 2.0f) * scale;
    float translate_y = height / 2.0f - ((max_y - min_y) / 2.0f) * scale;

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
	set_default_size(500, 250);
    set_child(ink_area);
}

MyApplication::MyApplication() : Gtk::Application("org.gtkmm.inkmlviewer", Gio::Application::Flags::HANDLES_OPEN) {}

Glib::RefPtr<MyApplication> MyApplication::create() {
    return Glib::make_refptr_for_instance<MyApplication>(new MyApplication());
}

// Convert local paths from args into global ones
std::string resolve(const std::string& input_path) {
    try {
        // If path is already absolute, return as-is
        if (std::filesystem::path(input_path).is_absolute()) {
            return input_path;
        }

        // Get current working directory
        std::filesystem::path current_path = std::filesystem::current_path();

        // Resolve the relative path
        std::filesystem::path full_path = std::filesystem::absolute(
            current_path / input_path
        );

        return full_path.string();
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Path resolution error: " << e.what() << std::endl;
        return input_path;
    }
}

MyWindow* MyApplication::create_appwindow(const std::string filepath) {
    std::string parsed_ground_truth;
    std::vector<std::vector<std::tuple<float, float>>> parsed_strokes;
    try {
        MyXMLParser parser;
        parser.set_substitute_entities(true);
        parser.parse_file(resolve(filepath));
        
        // Log parsed ground truth and stroke data
        parsed_ground_truth = parser.get_ground_truth();
        parsed_strokes = parser.get_strokes();
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return nullptr;
    }
    MyWindow *appwindow = new MyWindow(parsed_ground_truth, parsed_strokes);
    add_window(*appwindow);
    // Delete the window when it is hidden.
    appwindow->signal_hide().connect([appwindow](){ delete appwindow; });
    return appwindow;
}

void MyApplication::on_activate() {
    const auto data_dirs = Glib::get_system_data_dirs();

    std::string filepath;
    for (const auto& dir : data_dirs) {
        auto candidate = Glib::build_filename(dir, "inkmlviewer", "example.inkml");
        if (Glib::file_test(candidate, Glib::FileTest::EXISTS)) {
            filepath = candidate;
            break;
        }
    }
    if (filepath.empty()) {
        std::cerr << "example.inkml not found in system data directories." << std::endl;
    }
    auto appwindow = create_appwindow(filepath);
    appwindow->present();
}

void MyApplication::on_open(const Gio::Application::type_vec_files& files,
                            const Glib::ustring& /* hint */) {
    MyWindow* appwindow = nullptr;
    auto windows = get_windows();
    if (windows.size() > 0) {
        appwindow = dynamic_cast<MyWindow*>(windows[0]);
    }
    if (!appwindow) {
        appwindow = create_appwindow(files[0]->get_path());
    }
    appwindow->present();
}
