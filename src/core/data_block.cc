//---------------------------------------------------------------------------
#include <arpa/inet.h>
#include "data_block.h"
//---------------------------------------------------------------------------
namespace core
{

DataBlock::DataBlock(std::vector<char>&& data)
:   data_(std::move(data))
{
    data_begin_ = data_.data();
    data_end_ = data_begin_ + data_.size();

    return;
}
//---------------------------------------------------------------------------
bool DataBlock::Parse()
{
    //根据message格式，一个包至少含有3个字节
    if(3 > data_.size())
        return false;
    
    if(false == VerifyCAT())
        return false;

    if(false == GetDataRecordLen())
        return false;

    do
    {
        std::vector<uint8_t> fspec = GetFSPEC();
        if(fspec.empty())
            return true;

        DataRecord data_record(fspec, data_begin_, record_len_);
        data_begin_ = data_record.ParseRecord();
        if(nullptr == data_begin_)
            return false;
        records_.push_back(data_record);
    }while(data_end_!=data_begin_);

    return true;
}
//---------------------------------------------------------------------------
bool DataBlock::VerifyCAT()
{
    if(data_[0] != CAT_DATA_BLOCK)
        return false;

    data_begin_++;
    return true;
}
//---------------------------------------------------------------------------
bool DataBlock::GetDataRecordLen()
{
    uint16_t nlen = *reinterpret_cast<uint16_t*>(data_begin_);
    record_len_ = ntohs(nlen);

    //校验数据长度
    if(data_.size() != record_len_)
        return false;

    //如果等于3也没什么解析的必要了
    if(3 == record_len_)
        return false;

    data_begin_ += sizeof(record_len_);
    record_len_ = static_cast<uint16_t>(record_len_ - (sizeof(CAT_DATA_BLOCK)+sizeof(record_len_)));
    return true;
}
//---------------------------------------------------------------------------
std::vector<uint8_t> DataBlock::GetFSPEC()
{
    std::vector<uint8_t> fspec;

    uint8_t byte;
    do
    {
        byte = *data_begin_;
        fspec.push_back(byte);
        data_begin_++;
        record_len_--;
    }while(CheckFSPECEnd(byte));

    return fspec;
}
//---------------------------------------------------------------------------
bool DataBlock::CheckFSPECEnd(char fspec)
{
    char indicator = 0x01;
    if(!(indicator & fspec))
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

}//namespace core
//---------------------------------------------------------------------------

