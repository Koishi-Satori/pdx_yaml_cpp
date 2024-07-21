//
// Created by koishi on 2024/7/20.
//

#include "pdx_yaml_py.h"

extern "C" {
pdx_yaml::lexer *init_lexer() {
    return new pdx_yaml::lexer();
}
pdx_yaml::parser *init_parser() {
    return new pdx_yaml::parser();
}
int lexer_set_handle_bom(pdx_yaml::lexer *lexer, bool enable) {
    lexer->handle_bom(enable);
    return enable;
}
int lexer_load_file(pdx_yaml::lexer *lexer, const char *filename) {
    lexer->open_stream(filename);
    return !lexer->eof();
}
int lexer_close_file(pdx_yaml::lexer *lexer) {
    lexer->clear();
    return 1;
}
int parser_load_lexer(pdx_yaml::parser *parser, pdx_yaml::lexer *lexer) {
    parser->load(lexer);
    if (parser->ready())
        return 1;
    return 0;
}
pdx_yaml::pdx_localisation *parse(pdx_yaml::parser *parser) {
    return parser->parse();
}
const char *get_language(pdx_yaml::pdx_localisation *localisation) {
    return localisation->language;
}
std::vector<const char *> *get_comments(pdx_yaml::pdx_localisation *localisation) {
    return localisation->comments;
}
std::vector<pdx_yaml::entry *> *get_entries(pdx_yaml::pdx_localisation *localisation) {
    return localisation->entries;
}
unsigned long get_comments_size(std::vector<const char *> *comments) {
    return comments->size();
}
unsigned long get_entries_size(std::vector<pdx_yaml::entry *> *entries) {
    return entries->size();
}
const char *get_comment(std::vector<const char *> *comments, unsigned long index) {
    return comments->at(index);
}
pdx_yaml::entry *get_entry(std::vector<pdx_yaml::entry *> *entries, unsigned long index) {
    return entries->at(index);
}
const char *get_entry_key(pdx_yaml::entry *entry) {
    return entry->key;
}
const char *get_entry_value(pdx_yaml::entry *entry) {
    return entry->value;
}
int free_ptr(void *ptr) {
    if (ptr != nullptr) {
        free(ptr);
        return 1;
    }
    return 0;
}
}
