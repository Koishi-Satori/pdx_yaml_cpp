#ifndef PDX_YAML_NATIVE_PDX_YAML_H
#define PDX_YAML_NATIVE_PDX_YAML_H

#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iterator>
#include <queue>
#include <iostream>
#include <sstream>

namespace pdx_yaml {
    class pdx_yaml_syntax_error : public std::exception {
    public:
        const char *message;

        pdx_yaml_syntax_error() : message("Syntax Error") {}

        explicit pdx_yaml_syntax_error(const char *msg) : message(msg) {}

        [[nodiscard]]
        const char *what() const noexcept override {
            return message;
        }
    };

    enum token_type {
        COLON, STRING, LANGUAGE, NUMBER, COMMENT
    };
    struct token {
        token_type type;
        const char *content;
    };

    struct entry {
        const char *key;
        const char *value;

        friend std::ostream &operator<<(std::ostream &os, const entry &entry);
    };

    struct pdx_localisation {
        const char *language;
        std::vector<const char *> *comments{};
        std::vector<struct entry *> *entries{};

        virtual ~pdx_localisation();
    };

    /**
     * @brief a lexer for Paradox type yaml.
     * @author kkoishi_
     *
     * A lexer is used to produce tokens of a pdx yaml file.
     *
     * To use this, you should initialize it first, like: <a> pdx_yaml::lexer lexer <a/>.
     * Then use pdx_yaml::lexer::open_stream to open a file, and use pdx_yaml::lexer::scan
     * to scan for next token.
     * This lexer also supports handling BOM, that is the function pdx_yaml::lexer::handle_bom.
     *
     */
    class lexer {
    private:
        std::ifstream fs;
        std::queue<char> pre_peek;
        char peek;
        std::string buf;

        bool (*skip_line_sep)(char peek);

        unsigned long line = 0;
        unsigned long column = 0;
        bool skip_bom = false;

        void release();

        bool has_next();

        char next_char();

        token *language();

        token *value_string();

        token *comment();

        token *number();

        token *string();

    public:
        lexer();

        ~lexer() { release(); }

        void open_stream(const char *file_name);

        /**
         * Switch whether the lexer will handle the BOM.
         * @param enable a bool.
         */
        void handle_bom(bool enable);

        /**
         * Check if failed to open the file.
         * @return true if failed to open file.
         */
        bool is_open_failed();

        /**
         * Scan for next token.
         * @return a ptr of token, or nullptr if no more token rest
         */
        token *scan();

        /**
         * Close the stream and clear the lexer data.
         */
        void clear();

        /**
         * Check if the lexer has reach the EOF of the file.
         * @return true if eof bit is set.
         */
        bool eof();

        /**
         * The line number of current peek char.
         * @return unsigned long
         */
        unsigned long get_line() const;

        /**
         * The column number of current peek char.
         * @return unsigned long
         */
        unsigned long get_column() const;
    };

    class parser {
    private:
        void release();

        void verify(token_type required_type, const char *message);

        void language_tag(pdx_localisation *instance);

        void entries(pdx_localisation *instance);

        void build_entry(pdx_localisation *instance);

        lexer *_lexer;

        token *current;

    public:
        parser();

        ~parser() { release(); }

        void load(lexer *_l);

        pdx_localisation *parse();

        bool ready();
    };

    const char *get_token_type_name(token_type type) {
        switch (type) {
            case COLON:
                return "COLON";
            case STRING:
                return "STRING";
            case LANGUAGE:
                return "LANGUAGE";
            case NUMBER:
                return "NUMBER";
            case COMMENT:
                return "COMMENT";
            default:
                return "UNDEFINED";
        }
    }
}

#endif //PDX_YAML_NATIVE_PDX_YAML_H
