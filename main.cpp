#include <gtkmm-4.0/gtkmm.h>
#include <iostream>
#include <string>
#include <cstdlib>

#include "myxmlparser.h"
#include "inkmlviewer.h"

int main(int argc, char* argv[]) {
    // Open an example inkml file
    std::string filename = "example.inkml";

    // Parse the file
    try {
        MyXMLParser parser;
        parser.set_substitute_entities(true);
        parser.parse_file(filename);
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    auto app = Gtk::Application::create("org.gtkmm.inkmlviewer");
    return app->make_window_and_run<MyWindow>(argc, argv);
}
