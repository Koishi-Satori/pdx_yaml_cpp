#include <cstring>
#include "pdx_yaml.h"

namespace pdx_yaml {
    const char *copy_str(const std::string &str) {
        char *msg;
        msg = (char *) malloc((str.length() + 1) * sizeof(char));
        //str.copy(msg, str.length(), 0); // fuck this shit function.
        strcpy(msg, str.c_str());
        return msg;
    }

    void parser::release() {
    }

    parser::parser() {
        _lexer = nullptr;
        current = nullptr;
    }

    void parser::load(lexer *_l) {
        this->_lexer = _l;
    }

    bool parser::ready() {
        return !_lexer->eof();
    }

    pdx_localisation *parser::parse() {
        // std::cout << (_lexer != nullptr) << std::endl;
        auto instance = new pdx_localisation{};
        instance->comments = new std::vector<const char *>{};
        instance->entries = new std::vector<entry *>{};
        language_tag(instance);
        entries(instance);
        return instance;
    }

    void parser::language_tag(pdx_localisation *instance) {
        // just in case, skip any COMMENT tokens.
        while (true) {
            current = _lexer->scan();
            if (current == nullptr)
                break;
            if (current->type == COMMENT)
                continue;
            else
                break;
        }
        // if current is nullptr, maybe we meet an empty file.
        // so just return.
        if (current == nullptr) {
            instance->language = "EMPTY_FILE";
            return;
        }
        verify(LANGUAGE, "Require a language tag");
        instance->language = current->content;
        current = _lexer->scan();
        verify(COLON, "A language tag should contains a colon");
    }

    void parser::entries(pdx_localisation *instance) {
        while (true) {
            current = _lexer->scan();
            if (current == nullptr)
                break;
            if (current->type == COMMENT) {
                instance->comments->push_back(current->content);
            } else
                build_entry(instance);
        }
    }

    void parser::build_entry(pdx_localisation *instance) {
        verify(STRING, "A pdx yaml entry require a string as its key");
        auto p = new entry{};
        p->key = current->content;
        current = _lexer->scan();
        verify(COLON, "A pdx yaml entry require a colon here");
        current = _lexer->scan();
        if (current != nullptr && current->type == NUMBER) {
            current = _lexer->scan();
        }
        verify(STRING, "A pdx yaml entry require a string as its value");
        p->value = current->content;
        instance->entries->push_back(p);
    }

    void parser::verify(token_type required_type, const char *message) {
        if (current == nullptr || current->type != required_type) {
            std::stringstream ss;
            ss << message << " but got ";
            if (current != nullptr)
                ss << "<" << get_token_type_name(current->type) << ", " << current->content << ">";
            else
                ss << "NO TOKEN";
            ss << " at " << _lexer->get_line() << ":" << _lexer->get_column();
//            auto str = ss.str();
//            char *msg;
//            msg = (char *) malloc(str.length() * sizeof(char));
//            str.copy(msg, str.length(), 0);
            throw pdx_yaml_syntax_error(copy_str(ss.str()));
        }
    }

    void lexer::release() {
        if (fs.is_open()) {
            fs.close();
            fs.clear();
        }
        line = 0;
        column = 0;
        buf.clear();
        while (!pre_peek.empty())
            pre_peek.pop();
    }

    void lexer::open_stream(const char *file_name) {
        if (fs.is_open()) {
            fs.close();
            fs.clear();
        }
        line = 0;
        column = 0;
        buf.clear();
        while (!pre_peek.empty())
            pre_peek.pop();
        fs.open(file_name, std::ios::binary);
        // try to read utf8 with bom.
        // 0xEF, 0xBB, 0xBF
        if (skip_bom && !fs.fail()) {
            char bom[3] = {0};
            fs.read(bom, sizeof(char) * 3);
//            if ((unsigned char) bom[0] == 0xEF && (unsigned char) bom[1] == 0xBB && (unsigned char) bom[2] == 0xBF)
//                std::cout << "find bom." << std::endl;
        }
    }

    bool lexer::is_open_failed() {
        return fs.fail();
    }

    token *lexer::scan() {
        while (true) {
            if (!has_next())
                return nullptr;
            peek = next_char();
            if (std::isspace(peek)) {
                if (skip_line_sep(peek)) {
                    ++line;
                    column = 0;
                }
                continue;
            }
            if (!skip_line_sep(peek))
                break;
        }
        switch (peek) {
            case 'l':
                return language();
            case ':':
                return new token{COLON, nullptr};
            case '"':
                return value_string();
            case '#':
                return comment();
            default:
                if (std::isdigit(peek)) {
                    return number();
                } else
                    return string();
        }
    }

    bool skip_line_sep_linux(char peek) {
        return peek == '\n';
    }

    bool skip_line_sep_macos(char peek) {
        return peek == '\r';
    }

    bool skip_line_sep_win32(char peek) {
        return peek == '\n';
    }

    lexer::lexer() {
        peek = '\u0000';
#ifdef _WIN32
        // windows
        skip_line_sep = skip_line_sep_win32;
#elif __linux__
        // linux
        skip_line_sep = skip_line_sep_linux;
#elif __APPLE__
        // maxos
        skip_line_sep = skip_line_sep_macos;
#else
        // just consider as linux
        skip_line_sep = skip_line_sep_linux;
#endif
    }

    bool lexer::has_next() {
        return !pre_peek.empty() || (!fs.eof());
    }

    char lexer::next_char() {
        column++;
        if (!pre_peek.empty()) {
            auto result = pre_peek.front();
            pre_peek.pop();
            return result;
        }
        if (!fs.eof()) {
            return (char) fs.get();
        }
        return 0;
    }

    token *lexer::language() {
        buf.clear();
        buf += peek;
        while (has_next()) {
            peek = next_char();
            if (peek == ':' || std::isspace(peek) || peek == '\r' || peek == '\n') {
                pre_peek.push(peek);
                break;
            }
            buf += peek;
        }
        auto *_token = new token{};
        _token->content = copy_str(buf);
        if (buf == "l_english" || buf == "l_braz_por" || buf == "l_german" || buf == "l_french" ||
            buf == "l_spanish" || buf == "l_polish" || buf == "l_russian" || buf == "l_simp_chinese" ||
            buf == "l_japanese" || buf == "l_korean")
            _token->type = LANGUAGE;
        else
            _token->type = STRING;
        return _token;
    }

    token *lexer::value_string() {
        buf.clear();
        buf += peek;
        while (has_next()) {
            peek = next_char();
            if (peek == '#' || peek == '\r' || peek == '\n') {
                pre_peek.push(peek);
                break;
            }
            buf += peek;
        }
        if (buf.ends_with('"')) {
            auto quote_index = buf.find_last_of('"');
            buf.erase(quote_index, buf.length() - quote_index);
        }
        if (buf.starts_with('"'))
            buf.erase(buf.begin());
        return new token{STRING, copy_str(buf)};
    }

    token *lexer::comment() {
        buf.clear();
        buf += peek;
        while (has_next()) {
            peek = next_char();
            if (peek == '\r' || peek == '\n') {
                pre_peek.push(peek);
                break;
            }
            buf += peek;
        }
        return new token{COMMENT, copy_str(buf)};
    }

    token *lexer::number() {
        buf.clear();
        buf += peek;
        bool key_flag = false;
        while (has_next()) {
            peek = next_char();
            if (!std::isdigit(peek)) {
                if (!std::isspace(peek) && peek != ':' && peek != '"') {
                    if (!key_flag)
                        key_flag = true;
                    buf += peek;
                    continue;
                }
                if (peek == ':')
                    key_flag = true;
                pre_peek.push(peek);
                break;
            }
            buf += peek;
        }
        if (key_flag)
            return new token{STRING, copy_str(buf)};
        return new token{NUMBER, copy_str(buf)};
    }

    token *lexer::string() {
        if (peek == EOF)
            return nullptr;
        buf.clear();
        buf += peek;
        while (has_next()) {
            peek = next_char();
            if (peek == ':' || std::isspace(peek) || peek == '\r' || peek == '\n') {
                pre_peek.push(peek);
                break;
            }
            buf += peek;
        }
        return new token{STRING, copy_str(buf)};
    }

    void lexer::clear() {
        release();
    }

    bool lexer::eof() {
        return fs.eof();
    }

    unsigned long lexer::get_line() const {
        return line;
    }

    unsigned long lexer::get_column() const {
        return column;
    }

    void lexer::handle_bom(bool enable) {
        skip_bom = enable;
    }

    std::ostream &operator<<(std::ostream &os, const entry &entry) {
        os << entry.key << ": " << entry.value;
        return os;
    }

    pdx_localisation::~pdx_localisation() {
        delete language;
        for (const auto &item : *comments) {
            delete item;
        }
        for (const auto &item : *entries) {
            delete item;
        }
        comments->clear();
        entries->clear();
        delete comments;
        delete entries;
    }
}
