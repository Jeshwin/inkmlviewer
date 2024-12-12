#include <gtkmm-4.0/gtkmm.h>
#include <iostream>
#include <string>
#include <cstdlib>

#include "myxmlparser.h"
#include "inkmlviewer.h"

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.gtkmm.inkmlviewer", Gio::Application::Flags::HANDLES_OPEN);
    // Open an example inkml file
    std::string filepath = "example.inkml";
    if (argc > 1) {
        filepath = argv[1];
    }

    // Parse the file
    std::string parsed_ground_truth;
    std::vector<std::vector<std::tuple<float, float>>> parsed_strokes;
    try {
        MyXMLParser parser;
        parser.set_substitute_entities(true);
        parser.parse_file(filepath);
        
        // Log parsed ground truth and stroke data
        parsed_ground_truth = parser.get_ground_truth();
        parsed_strokes = parser.get_strokes();
        std::cout << parsed_ground_truth << '\n';
        for (auto stroke : parsed_strokes) {
            std::cout << '<';
            for (auto coord : stroke) {
                std::cout << '(' << std::get<0>(coord) << ", " << std::get<1>(coord) << "), ";
            }
            std::cout << ">\n";
        }
        std::cout << "Found " << parsed_strokes.size() << " strokes\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Displaying app now...\n";
    return app->make_window_and_run<MyWindow>(argc, argv, parsed_ground_truth, parsed_strokes);
}
