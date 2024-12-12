#ifndef _GTK_MM_WINDOW
#define _GTK_MM_WINDOW

#include <gtkmm-4.0/gtkmm/window.h>
#include <gtkmm-4.0/gtkmm/drawingarea.h>
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

class MyWindow : public Gtk::Window {
public:
    MyWindow(const std::string ground_truth, const std::vector<std::vector<std::tuple<float, float>>>& strokes);
protected:
    InkArea ink_area;
};

#endif // _GTK_MM_WINDOW
