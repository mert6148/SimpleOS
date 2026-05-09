#if

enum class ValueParserType {
    Unknown,
    Function,
    Word,
    String,
    Space,
    Comment,
    Div
};

struct ValueParserNode {
    ValueParserType type;
    std::string value;
    std::vector<ValueParserNode> nodes;
};

class ValueParser {
public:
    ValueParser(const std::string& input);
    std::vector<ValueParserNode> parse();
};

#endif // POSTCSS_VALUE_PARSER_HPP