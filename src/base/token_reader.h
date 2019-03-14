//---------------------------------------------------------------------------
#ifndef JSON_SRC_TOKEN_READER_H_
#define JSON_SRC_TOKEN_READER_H_
//---------------------------------------------------------------------------
#include "value.h"
#include "char_reader.h"
//---------------------------------------------------------------------------
namespace json
{

class Value;

class TokenReader
{
public:
    TokenReader()
    {
    }
    ~TokenReader()
    {
    }

    //设置要分析的数据
    void set_dat(std::string&& dat) { char_reader_.set_dat(std::move(dat)); }

    //token类型
    enum TokenType
    {
        DOCUMENT_END = 1,   // 分析结束
        OBJECT_BEGIN,       // obj开始
        OBJECT_END,         // obj结束
        ARRAY_BEGIN,        // array开始
        ARRAY_END,          // array结束
        SEP_COLON,          // 分隔符 :
        SEP_COMMA,          // 分隔符 ,
        STRING,             // 字符串
        BOOLEAN,            // 布尔值
        NUMBER,             // 数值
        NUL,                // 空
        INVALID             // 非法的字符
    };

    //读取下一个Token
    TokenType ReadNextToken();
    
    //读取字符串的值
    bool ReadString(std::string& str);

    //读取数值类型
    bool ReadNumber(std::string& num, Value::TYPE& type);

    //布尔类型
    bool ReadBoolean(bool& boolean);

    //空类型
    bool ReadNull();

private:
    //判断是否空白字符
    bool IsWhitespace(char c)
    {
        return ((' '==c) || ('\t'==c) || ('\n'==c) || ('\r'==c));
    }

    void SkipWhitespace();

    bool CheckNumber(char c)
    {
        if((isdigit(c)) || ('.'==c) || ('-'==c))// || ('e'==c) || ('E'==c))暂时不解析e
            return true;

        return false;
    }

private:
    CharReader char_reader_;
};

}//namespace json
//---------------------------------------------------------------------------
#endif //JSON_SRC_TOKEN_READER_H_
