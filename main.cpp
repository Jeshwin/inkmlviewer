#include <gtkmm-4.0/gtkmm.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include "inkmlviewer.hpp"
#include "myxmlparser.hpp"

int main(int argc, char* argv[]) {
    // First, check number of args
    if (argc > 2) {
        std::cout << "Usage: inkmlviewer [FILE]" << std::endl;
        return EXIT_FAILURE;
    }
    // Open an example inkml file
    std::string filepath = "example.inkml";
    if (argc == 2) {
        filepath = argv[1];
    }
    filepath = resolve(filepath);
    std::cout << filepath << '\n';

    std::cout << "Displaying app now...\n";
    auto app = MyApplication::create();
    return app->run(argc, argv);
}
