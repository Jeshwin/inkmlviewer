#include "inkmlviewer.h"
#include <gtkmm-4.0/gtkmm.h>
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    // Open an example inkml file
    std::string filename = "example.inkml";
    std::ifstream example_file(filename);

    // Read out the file and print to stdout
    std::string curr_line;
    while (std::getline(example_file, curr_line)) {
        std::cout << curr_line << '\n';
    }

;	auto app = Gtk::Application::create("org.gtkmm.inkmlviewer");
	return app->make_window_and_run<MyWindow>(argc, argv);
}
