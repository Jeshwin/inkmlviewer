#ifndef __LIBXMLPP_EXAMPLES_MYPARSER_H
#define __LIBXMLPP_EXAMPLES_MYPARSER_H

#include <libxml++-5.0/libxml++/libxml++.h>

class MyXMLParser : public xmlpp::SaxParser {
public:
    MyXMLParser();
    ~MyXMLParser();

protected:
    //overrides:
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
};

#endif //__LIBXMLPP_EXAMPLES_MYPARSER_H
