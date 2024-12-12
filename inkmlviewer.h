#ifndef _GTK_MM_INKML_VIEWER
#define _GTK_MM_INKML_VIEWER

#include <gtkmm-4.0/gtkmm.h>
#include <string>
#include <tuple>
#include <vector>

class InkArea : public Gtk::DrawingArea {
public:
    InkArea(const std::string ground_truth, const std::vector<std::vector<std::tuple<float, float>>>& strokes);
    virtual ~InkArea();
protected:
    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
    std::string ground_truth;
    std::vector<std::vector<std::tuple<float, float>>> strokes;
};

class MyWindow : public Gtk::ApplicationWindow {
public:
    MyWindow(const std::string ground_truth, const std::vector<std::vector<std::tuple<float, float>>>& strokes);
protected:
    InkArea ink_area;
};

class MyApplication : public Gtk::Application {
public:
    static Glib::RefPtr<MyApplication> create();

protected:
    MyApplication();
    void on_activate();
    void on_open(const Gio::Application::type_vec_files& files, const Glib::ustring& hint);

private:
    MyWindow* create_appwindow(const std::string filepath);
};

// Convert local paths from args into global ones
std::string resolve(const std::string& input_path);

#endif // _GTK_MM_INKML_VIEWER
