//
// Created by koishi on 2024/7/20.
//

#ifndef PDX_YAML_NATIVE_PDX_YAML_PY_H
#define PDX_YAML_NATIVE_PDX_YAML_PY_H

#include "pdx_yaml.h"

extern "C" {
pdx_yaml::lexer *init_lexer();
pdx_yaml::parser *init_parser();
int lexer_set_handle_bom(pdx_yaml::lexer *lexer, bool enable);
int lexer_load_file(pdx_yaml::lexer *lexer, const char *filename);
int lexer_close_file(pdx_yaml::lexer *lexer);
int parser_load_lexer(pdx_yaml::parser *parser, pdx_yaml::lexer *lexer);
pdx_yaml::pdx_localisation *parse(pdx_yaml::parser *parser);
const char *get_language(pdx_yaml::pdx_localisation *localisation);
std::vector<const char *> *get_comments(pdx_yaml::pdx_localisation *localisation);
std::vector<pdx_yaml::entry *> *get_entries(pdx_yaml::pdx_localisation *localisation);
unsigned long get_comments_size(std::vector<const char *> *comments);
unsigned long get_entries_size(std::vector<pdx_yaml::entry *> *entries);
const char *get_comment(std::vector<const char *> *comments, unsigned long index);
pdx_yaml::entry *get_entry(std::vector<pdx_yaml::entry *> *entries, unsigned long index);
const char *get_entry_key(pdx_yaml::entry *entry);
const char *get_entry_value(pdx_yaml::entry *entry);
int free_ptr(void *ptr);
};

#endif //PDX_YAML_NATIVE_PDX_YAML_PY_H
