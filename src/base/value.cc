//---------------------------------------------------------------------------
#include "value.h"
#include "json_writer.h"
//---------------------------------------------------------------------------
namespace json
{
//---------------------------------------------------------------------------
const Value Value::kValueNull = Value(NUL);
//---------------------------------------------------------------------------
Value::Value()
:   type_(NUL),
    val_("null"),
    array_(0),
    pairs_(0)
{
    return;
}
//---------------------------------------------------------------------------
Value::Value(TYPE val_type)
:   type_(val_type),
    val_("null"),
    array_(0),
    pairs_(0)
{
    if(type_ == OBJECT) { pairs_ = new Pair; return; }
    if(type_ == ARRAY)  { array_ = new Array; return; }

    return;
}
//---------------------------------------------------------------------------
Value::Value(const std::string& value)
:   type_(STRING),
    val_(value),
    array_(0),
    pairs_(0)
{
    return;
}
//---------------------------------------------------------------------------
Value::Value(std::string&& value)
:   type_(STRING),
    val_(std::move(value)),
    array_(0),
    pairs_(0)
{
    return;
}
//---------------------------------------------------------------------------
Value::Value(const char* value)
:   type_(STRING),
    val_(value),
    array_(0),
    pairs_(0)
{
    return;
}
//---------------------------------------------------------------------------
Value::Value(int value)
:   type_(INT),
    array_(0),
    pairs_(0)
{
    set_int(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(int64_t value)
:   type_(INT),
    array_(0),
    pairs_(0)
{
    set_int(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(uint64_t value)
:   type_(UINT),
    array_(0),
    pairs_(0)
{
    set_uint(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(double value)
:   type_(REAL),
    array_(0),
    pairs_(0)
{
    set_double(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(bool value)
:   type_(BOOLEAN),
    array_(0),
    pairs_(0)
{
    set_boolean(value);
    return;
}
//---------------------------------------------------------------------------
Value::Value(const Value& other)
:   type_(other.type_),
    val_(other.val_),
    array_(0),
    pairs_(0)
{
    switch(other.type_)
    {
        case OBJECT:
            pairs_ = new Pair(*(other.pairs_));
            break;

        case ARRAY:
            array_  = new Array(*(other.array_));
            break;

        case KEY:
        case STRING:
        case INT:
        case UINT:
        case REAL:
        case BOOLEAN:
        case NUL:
            break;

        default:
            assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
Value::Value(Value&& other)
:   type_(other.type_),
    val_(std::move(other.val_)),
    array_(other.array_),
    pairs_(other.pairs_)
{
    other.type_ = NUL;
    other.val_  = "null";
    other.pairs_= 0;
    other.array_= 0;

    return;
}
//---------------------------------------------------------------------------
Value& Value::operator=(const Value& other)
{
    if(this == &other)
        return *this;

    if(type_ == TYPE::OBJECT)   { delete pairs_; pairs_ = 0; };
    if(type_ == TYPE::ARRAY)    { delete array_; array_ = 0; };

    type_ = other.type_;
    val_ = other.val_;

    switch(other.type_)
    {
        case OBJECT:
            pairs_ = new Pair(*(other.pairs_));
            break;

        case ARRAY:
            array_  = new Array(*(other.array_));
            break;

        case KEY:
        case STRING:
        case INT:
        case UINT:
        case REAL:
        case BOOLEAN:
        case NUL:
            break;

        default:
            assert(0);
    }

    return *this;
}
//---------------------------------------------------------------------------
Value& Value::operator=(Value&& other)
{
    if(type_ == TYPE::OBJECT)   { delete pairs_; pairs_ = 0; };
    if(type_ == TYPE::ARRAY)    { delete array_; array_ = 0; };

    type_ = other.type_;
    val_ = std::move(other.val_);

    other.val_ = "null";
    switch(other.type_)
    {
        case OBJECT:
            pairs_        = other.pairs_;
            other.pairs_  = 0;
            break;

        case ARRAY:
            array_      = other.array_;
            other.array_= 0;
            break;

        case KEY:
        case STRING:
        case INT:
        case UINT:
        case REAL:
        case BOOLEAN:
        case NUL:
            break;

        default:
            assert(0);
    }

    other.type_ = NUL;
    return *this;
}
//---------------------------------------------------------------------------
Value::~Value()
{
    if(NUL == type_)
    {
        assert(0 == array_);
        assert(0 == pairs_);
        assert("null" == val_);
        return;
    }

    switch(type_)
    {
        case OBJECT:
            delete pairs_;
            break;

        case ARRAY:
            delete array_;
            break;
        
        case KEY:
        case STRING:
        case INT:
        case UINT:
        case REAL:
        case BOOLEAN:
        case NUL:
            break;

        default:
            assert(0);
    }

    return;
}
//---------------------------------------------------------------------------
Value& Value::PairAdd(const std::string& key, const Value& value)
{
    return PairAdd(key.c_str(), value);
}
//---------------------------------------------------------------------------
Value& Value::PairAdd(std::string&& key, const Value& value)
{
    assert(0 != pairs_);

    auto& ret = (*pairs_)[std::move(key)];
    ret = value;
    return ret;
}
//---------------------------------------------------------------------------
Value& Value::PairAdd(const char* key, const Value& value)
{
    assert(0 != pairs_);

    auto& ret = (*pairs_)[key];
    ret = value;
    return ret;
}
//---------------------------------------------------------------------------
Value& Value::PairAdd(const std::string& key, Value&& value)
{
    return PairAdd(key.c_str(), std::move(value));
}
//---------------------------------------------------------------------------
Value& Value::PairAdd(std::string&& key, Value&& value)
{
    assert(0 != pairs_);

    auto& ret = (*pairs_)[std::move(key)];
    ret = std::move(value);
    return ret;
}
//---------------------------------------------------------------------------
Value& Value::PairAdd(const char* key, Value&& value)
{
    assert(0 != pairs_);

    auto& ret = (*pairs_)[key];
    ret = std::move(value);
    return ret;
}
//---------------------------------------------------------------------------
bool Value::PairDel(const std::string& key)
{
    assert(0 != pairs_);

    size_t nums = pairs_->erase(key);
    return (1 == nums);
}
//---------------------------------------------------------------------------
bool Value::PairDel(const char* key)
{
    assert(0 != pairs_);

    size_t nums = pairs_->erase(key);
    return (1 == nums);
}
//---------------------------------------------------------------------------
bool Value::PairGet(const std::string& key, Value* value) const
{
    return PairGet(key.c_str(), value);
}
//---------------------------------------------------------------------------
bool Value::PairGet(const char* key, Value* value) const
{
    assert(0 != pairs_);

    auto iter = pairs_->find(key);
    if(pairs_->end() == iter)
        return false;

    *value = iter->second;
    return true;
}
//---------------------------------------------------------------------------
void Value::ArrayResize(size_t size)
{
    assert(0 != array_);

    array_->resize(size);
    return;
}
//---------------------------------------------------------------------------
void Value::ArraySet(size_t index, const Value& value)
{
    assert(0 != array_);

    array_->at(index) = value;
    return;
}
//---------------------------------------------------------------------------
void Value::ArraySet(size_t index, const Value&& value)
{
    assert(0 != array_);

    array_->at(index) = std::move(value);
    return;
}
//---------------------------------------------------------------------------
Value& Value::ArrayGet(size_t index)
{
    assert(0 != array_);

    return array_->at(index);
}
//---------------------------------------------------------------------------
const Value& Value::ArrayGet(size_t index) const
{
    assert(0 != array_);

    return array_->at(index); 
}
//---------------------------------------------------------------------------
void Value::ArrayAdd(const Value& value)
{
    assert(0 != array_);

    array_->push_back(value);
    return;
}
//---------------------------------------------------------------------------
void Value::ArrayAdd(Value&& value)
{
    assert(0 != array_);

    array_->push_back(std::move(value));
    return;
}
//---------------------------------------------------------------------------
void Value::ArrayZero(size_t index)
{
    assert(0 != array_);

    array_->at(index) = kValueNull;
    return;
}
//---------------------------------------------------------------------------
Value& Value::operator[](const char* key)
{
    assert(OBJECT == type_);
    
    Value& value = (*pairs_)[key];
    return value;
}
//---------------------------------------------------------------------------
Value& Value::operator[](const std::string& key)
{
    assert(OBJECT == type_);
    
    Value& value = (*pairs_)[key];
    return value;
}
//---------------------------------------------------------------------------
const Value& Value::operator[](const char* key) const
{
    assert(OBJECT == type_);
    
    const Value& value = (*pairs_)[key];
    return value;
}
//---------------------------------------------------------------------------
const Value& Value::operator[](const std::string& key) const
{
    assert(OBJECT == type_);
    
    const Value& value = (*pairs_)[key];
    return value;
}
//---------------------------------------------------------------------------
Value& Value::operator[](int index)
{
    assert(ARRAY == type_);

    return (*array_)[index];
}
//---------------------------------------------------------------------------
const Value& Value::operator[](int index) const
{
    assert(ARRAY == type_);

    return (*array_)[index];
}
//---------------------------------------------------------------------------
std::string Value::ToString(bool format)
{
    return JsonWriter(*this).ToString(format);
}
//---------------------------------------------------------------------------
void Value::set_type(TYPE type_val)
{
    assert(NUL == type_);

    type_ = type_val;

    if(type_ == OBJECT) { pairs_= new Pair; return; }
    if(type_ == ARRAY)  { array_ = new Array; return; }

    return;
}
//---------------------------------------------------------------------------
}//namespace json
