//---------------------------------------------------------------------------
#include "data_record.h"
//---------------------------------------------------------------------------
namespace core
{

//---------------------------------------------------------------------------
DataRecord::DataRecord(uint32_t fspec, char* data_begin, size_t data_block_len)
:   data_begin_(data_begin),
    data_block_len_(data_block_len),
    record_len_(0)
{
    fspec_ = fspec;
    actions_ = {
        std::bind(&DataRecord::ParseDataSourceIdentification, this),
        std::bind(&DataRecord::ParseTargetReportDescriptor, this),
        std::bind(&DataRecord::ParseTimeOfDay, this),
        std::bind(&DataRecord::ParsePositionWSG84, this),
        std::bind(&DataRecord::ParseTargetAddress, this),
        std::bind(&DataRecord::ParseGeometricAltitude, this),
        std::bind(&DataRecord::ParseFigureOfMerit, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),
        std::bind(&DataRecord::ParseNotImplement, this),
    };
}
//---------------------------------------------------------------------------
char* DataRecord::ParseRecord()
{
    uint32_t sign = 1;
    //FIXME:可以根据fspec_值范围判断循环8 16 24 32 times
    for(size_t i=0; i<sizeof(uint32_t); i++)
    {
        if(fspec_ & sign)
        {
            actions_[i]();
        }
        sign = sign << 1;
    }

    return nullptr;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseDataSourceIdentification()
{
    //数据长度不够，返回失败
    if((record_len_+2) > data_block_len_)
        return false;

    uint8_t buf[2];
    buf[0] = data_begin_[0];
    buf[1] = data_begin_[1];
    (void)buf;

    record_len_ += 2;
    data_begin_ += 2;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetReportDescriptor ()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTimeOfDay()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParsePositionWSG84()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetAddress()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseGeometricAltitude()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFigureOfMerit()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFieldExtensionIndicator()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseNotImplement()
{
    return true;
}
//---------------------------------------------------------------------------

}//namespace core
//---------------------------------------------------------------------------

