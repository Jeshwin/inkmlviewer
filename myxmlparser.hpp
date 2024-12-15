#ifndef __LIBXMLPP_EXAMPLES_MYPARSER_H
#define __LIBXMLPP_EXAMPLES_MYPARSER_H

#include <libxml++-5.0/libxml++/libxml++.h>

#include <string>
#include <tuple>
#include <vector>

class MyXMLParser : public xmlpp::SaxParser {
   public:
    MyXMLParser();
    ~MyXMLParser();
    std::string get_ground_truth();
    std::vector<std::vector<std::tuple<float, float>>> get_strokes();

   protected:
    // overrides:
    void on_start_document();
    void on_end_document();
    void on_start_element(const xmlpp::ustring& name,
                          const AttributeList& properties);
    void on_end_element(const xmlpp::ustring& name);
    void on_characters(const xmlpp::ustring& characters);
    void on_comment(const xmlpp::ustring& text);
    void on_warning(const xmlpp::ustring& text);
    void on_error(const xmlpp::ustring& text);
    void on_fatal_error(const xmlpp::ustring& text);

   private:
    std::string ground_truth;
    std::vector<std::vector<std::tuple<float, float>>> strokes;
    bool check_ground_truth = true;  // Unset flag once ground truth found
    bool in_ground_truth =
        false;               // Set flag so on_characters() sets ground_truth
    int curr_trace_id = -1;  // If we are in a trace element, set to its id
    // on_characters() will add to strokes if curr_trace_id >= 0
};

#endif  //__LIBXMLPP_EXAMPLES_MYPARSER_H
