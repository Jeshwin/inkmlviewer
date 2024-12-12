#include "myxmlparser.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>

MyXMLParser::MyXMLParser() : xmlpp::SaxParser() {}

MyXMLParser::~MyXMLParser() {}

std::string MyXMLParser::get_ground_truth() {
    return ground_truth;
}

std::vector<std::vector<std::tuple<float, float>>> MyXMLParser::get_strokes() {
    return strokes;
}

void MyXMLParser::on_start_document() {
    // std::cout << "on_start_document()" << std::endl;
}

void MyXMLParser::on_end_document() {
    // std::cout << "on_end_document()" << std::endl;
}

void MyXMLParser::on_start_element(const xmlpp::ustring& name, const AttributeList& attributes) {
    // std::cout << "node name=" << name << std::endl;

    // Print attributes:
    for (const auto& attr_pair : attributes) {
        // std::cout << "  Attribute name=" <<  attr_pair.name << std::endl;
        // std::cout << "    , value= " <<  attr_pair.value << std::endl;
        if (attr_pair.name == "type" && attr_pair.value == "truth" && check_ground_truth) {
            // std::cout << "Found ground truth LaTeX!\n";
            check_ground_truth = false;
            in_ground_truth = true;
        }
        if (name == "trace" && attr_pair.name == "id") {
            curr_trace_id = std::atoi(attr_pair.value.data());
        }
    } 
}

void MyXMLParser::on_end_element(const xmlpp::ustring& /* name */) {
  // std::cout << "on_end_element()" << std::endl;
}

void MyXMLParser::on_characters(const xmlpp::ustring& text) {
    // std::cout << "on_characters(): " << text << std::endl;
    if (in_ground_truth) {
        ground_truth = text;
        in_ground_truth = false;
    }
    if (curr_trace_id >= 0) {
        std::vector<std::tuple<float, float>> current_stroke;
        std::istringstream stroke_ss(text.data());\
        std::string coord_pair;
        while (std::getline(stroke_ss, coord_pair, ',')) {
            std::istringstream coord_pair_ss(coord_pair);
            float x, y;
            if (coord_pair_ss >> x >> y) {
                current_stroke.push_back(std::make_tuple(x, y));
            }
        }
        strokes.push_back(current_stroke);
        std::cout << "Parsed trace with " << current_stroke.size() << " points\n";
        curr_trace_id = -1; // Reset trace_id
    }
}

void MyXMLParser::on_comment(const xmlpp::ustring& text) {
  // std::cout << "on_comment(): " << text << std::endl;
}

void MyXMLParser::on_warning(const xmlpp::ustring& text) {
  // std::cout << "on_warning(): " << text << std::endl;
}

void MyXMLParser::on_error(const xmlpp::ustring& text) {
  std::cout << "on_error(): " << text << std::endl;
}

void MyXMLParser::on_fatal_error(const xmlpp::ustring& text) {
  std::cout << "on_fatal_error(): " << text << std::endl;
}
