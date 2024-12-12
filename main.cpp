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
        
        // Get ground truth?
        std::cout << parser.get_ground_truth() << '\n';
        const auto strokes = parser.get_strokes();
        for (auto stroke : strokes) {
            std::cout << '<';
            for (auto coord : stroke) {
                std::cout << '(' << std::get<0>(coord) << ", " << std::get<1>(coord) << "), ";
            }
            std::cout << ">\n";
        }
        std::cout << "Found " << strokes.size() << " strokes\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    auto app = Gtk::Application::create("org.gtkmm.inkmlviewer");
    return app->make_window_and_run<MyWindow>(argc, argv);
}
