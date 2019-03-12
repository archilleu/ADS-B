//---------------------------------------------------------------------------
#include <arpa/inet.h>
#include "data_block.h"
#include "data_record.h"
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
        uint32_t fspec = GetFSPEC();
        if(0 == fspec)
            return true;

        DataRecord data_record(fspec, data_begin_, record_len_);
        data_begin_ = data_record.ParseRecord();
        if(nullptr == data_begin_)
            return false;

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
uint32_t DataBlock::GetFSPEC()
{
    uint32_t fspec = 0;
    uint8_t byte;
    for(size_t i=0; i<4; i++)
    {
        byte = *data_begin_;
        uint32_t tmp = byte;
        tmp = tmp << (i*8);
        fspec += tmp;
        data_begin_++;
        record_len_--;
    
        if(!CheckFSPECEnd(byte))
            break;
    }

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

