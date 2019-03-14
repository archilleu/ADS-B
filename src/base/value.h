//---------------------------------------------------------------------------
#ifndef JSON_SRC_VALUE_H_
#define JSON_SRC_VALUE_H_
//---------------------------------------------------------------------------
#include <inttypes.h>
#include <vector>
#include <map>
#include <string>
#include <assert.h>
//---------------------------------------------------------------------------
namespace json
{

class Value
{
public:
    //Value的类型
    enum TYPE
    {
        KEY = 1,
        OBJECT,     //对象类型
        ARRAY,      //数组类型
        STRING,     //字符串对象
        INT,        //有符号整形
        UINT,       //无符号整形
        REAL,       //浮点型
        NUMBER,     //数字类型(INT,UINT,REAL)
        BOOLEAN,    //布伦
        NUL,        //空
    };

    //类型定义
    using Array     = std::vector<Value>;
    using Pair      = std::map<std::string, Value>;
    using ArrayIter = Array::const_iterator;
    using PairIter  = Pair::const_iterator;

public:
    Value();
    Value(TYPE type);
    Value(const std::string& value);
    Value(std::string&& value);
    Value(const char* value);
    Value(int value);
    Value(int64_t value);
    Value(uint64_t value);
    Value(double value);
    Value(bool value);
    Value(const Value& other);
    Value(Value&& other);
    Value& operator=(const Value& other);
    Value& operator=(Value&& other);
    ~Value();

public:
    TYPE type() const  { return type_; }

    //设置值
    void set_int(int64_t value)
    {
        assert(INT == type_);
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%" PRId64 "", value);
        val_ = buffer;
    }

    void set_uint(uint64_t value)
    {
        assert(UINT == type_);
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%" PRIu64 "", value);
        val_ = buffer;
    }

    void set_boolean(bool value)
    {
        assert(BOOLEAN == type_);
        value ? (val_="true") : (val_="false");
    }

    //使用17位精度
    void set_double(double value)
    {
        assert(REAL == type_);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%.17lf", value);
        val_ = buffer;
    }
    void set_str(const char* str)
    {
        if((STRING!=type_) && (KEY!=type_))
            assert(0);

        val_ = str;
    }
    void set_str(const std::string& str)
    {
        if((STRING!=type_) && (KEY!=type_))
            assert(0);
        
        val_ = str;
    }
    void set_str(std::string&& str)
    {
        if((STRING!=type_) && (KEY!=type_))
            assert(0);

        val_ = str;
    }

    //获取值
    int64_t             get_int()       const   { assert(INT == type_); return std::stoll(val_); }
    uint64_t            get_uint()      const   { assert(UINT == type_); return std::stoull(val_); }
    bool                get_boolean()   const   { assert(BOOLEAN == type_); return (val_ == "true"); }
    double              get_double()    const   { assert(REAL == type_); return std::stod(val_); }
    std::string&        val()                   { return val_; }
    const std::string&  val()           const   { return val_; }

    //Pair
    Value&  PairAdd (const std::string& key, const Value& value);
    Value&  PairAdd (std::string&& key, const Value& value);
    Value&  PairAdd (const char* key, const Value& value);
    Value&  PairAdd (const std::string& key, Value&& value);
    Value&  PairAdd (std::string&& key, Value&& value);
    Value&  PairAdd (const char* key, Value&& value);

    bool    PairDel (const std::string& key);
    bool    PairDel (const char* key);

    bool    PairGet (const std::string& key, Value* value) const;
    bool    PairGet (const char* key, Value* value) const;

    size_t  PairSize() const { if(0 == pairs_) assert(0);  return pairs_->size(); }

    PairIter PairIterBegin  () const    { return pairs_->begin(); }
    PairIter PairIterEnd    () const    { return pairs_->end(); }

    //array
    void            ArrayResize (size_t size);
    size_t          ArraySize   () const    { if(0 == array_) return 0; return array_->size(); }

    void            ArraySet    (size_t index, const Value& value);
    void            ArraySet    (size_t index, const Value&& value);

    Value&          ArrayGet    (size_t index);
    const Value&    ArrayGet    (size_t index) const;

    void            ArrayAdd    (const Value& value);
    void            ArrayAdd    (Value&& value);

    void            ArrayZero   (size_t index);

    ArrayIter ArrayIterBegin()  const   { return array_->begin(); }
    ArrayIter ArrayIterEnd()    const   { return array_->end(); }

//重载[]操作符
public:
    Value& operator[] (const char* key);
    Value& operator[] (const std::string& key);
    const Value& operator[] (const char* key) const;
    const Value& operator[] (const std::string& key) const;

    Value&          operator[] (int index); //参数类型原本为size_t,但是index=0和上面的const char* 有冲突，所以定义为int
    const Value&    operator[] (int index) const;

//输出Value字符串值
public:
    std::string ToString(bool format=false);

public:
    const static Value kValueNull;

private:
    //供json_reader调用
    friend class JsonReader;
    void set_type(TYPE type);
    void set_number(const std::string& number, TYPE num_type)   { val_ = number; type_ = num_type; }
    void set_number(const char* number, TYPE num_type)          {val_ = number; type_ = num_type; }
    void set_number(std::string&& number, TYPE num_type)        {val_ = std::move(number); type_ = num_type; }

private:
    TYPE                            type_;  //类型
    std::string                     val_;   //值
    std::vector<Value>*             array_; //数组
    std::map<std::string, Value>*   pairs_; //对象
};

}//namespace json
//---------------------------------------------------------------------------
#endif //JSON_SRC_VALUE_H_
