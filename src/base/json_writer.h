//---------------------------------------------------------------------------
#ifndef JSON_SRC_WRITER_H_
#define JSON_SRC_WRITER_H_
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
namespace json
{

class Value;

class JsonWriter
{
public:
    JsonWriter(const Value& val)
    :   val_(val)
    {
    }
    ~JsonWriter()
    {
    }

    std::string ToString(bool format=false);
    bool ToFile(const std::string& path, bool format=false);

private:
    void ToString        (const Value& value, std::string& str, bool format, int deep);
    void ObjectToString  (const Value& value, std::string& str, bool format, int deep);
    void ArrayToString   (const Value& value, std::string& str, bool format, int deep);

    static void AddTab(std::string& str, int deep)
    {
        for(int i=0; i<deep; i++)
            str.push_back('\t');
    }

    void AddString(const std::string& from, std::string& str);

private:
    const Value& val_;
};

}//namespace json

//---------------------------------------------------------------------------
#endif //JSON_SRC_WRITER_H_
