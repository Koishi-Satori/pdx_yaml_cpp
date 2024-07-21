//
// Created by koishi on 2024/7/19.
//

#include "pdx_yaml.h"
#include <chrono>

int main() {
    std::ofstream clear_s("./test.out", std::ios::trunc);
    clear_s.close();
    std::ofstream fs;
    fs.open("./test.out");
    if (fs.fail()) {
        std::cout << "failed to open out file." << std::endl;
        return -1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    pdx_yaml::lexer lexer;
    lexer.handle_bom(true);
    lexer.open_stream("./l_simp_chinese.yml");
    fs << lexer.is_open_failed() << std::endl;
    pdx_yaml::token *t;
    unsigned long c = 0;
    while ((t = lexer.scan()) != nullptr) {
        ++c;
        if (t->type == pdx_yaml::COLON)
            fs << c << " find token: COLON" << std::endl;
        else {
            fs << c << " find token: <";
            fs << pdx_yaml::get_token_type_name(t->type) << ", " << t->content << ">" << std::endl;
        }
        free(t);
    }
    lexer.clear();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << "Time: " << duration << "ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    pdx_yaml::lexer n_lexer;
    n_lexer.handle_bom(true);
    n_lexer.open_stream("./l_simp_chinese.yml");
    pdx_yaml::parser parser;
    parser.load(&n_lexer);
    auto result = parser.parse();
    fs << result->language << std::endl;
    for (const auto &item: *result->comments) {
        fs << item << std::endl;
    }
    for (const auto &item: *result->entries) {
        fs << *item << std::endl;
    }
    lexer.clear();
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << "Time: " << duration << "ms" << std::endl;
    // n_lexer.clear();
    n_lexer.open_stream("../localisation/japanese/paragon_triggers_l_japanese.yml");
    parser.parse();

    fs.close();
    fs.clear();
}
