//---------------------------------------------------------------------------
#ifndef CORE_DATA_RECORD_H_
#define CORE_DATA_RECORD_H_
//---------------------------------------------------------------------------
#include <cstdint>
#include <vector>
#include <functional>
//---------------------------------------------------------------------------
namespace core
{

class DataRecord
{
public:
    DataRecord(uint32_t fspec, char* data_begin, size_t data_block_len);
    char* ParseRecord();

public:
    const uint8_t DataSourceIdentification  = 0x01;
    const uint8_t TargetReportDescriptor    = 0x02;
    const uint8_t TimeOfDay                 = 0x04;
    const uint8_t PositionWSG84             = 0x08;
    const uint8_t TargetAddress             = 0x10;
    const uint8_t GeometricAltitude         = 0x20;
    const uint8_t FigureOfMerit             = 0x40;
    const uint8_t FieldExtensionIndicator   = 0x80;

private:
    bool ParseDataSourceIdentification();
    bool ParseTargetReportDescriptor ();
    bool ParseTimeOfDay();
    bool ParsePositionWSG84();
    bool ParseTargetAddress();
    bool ParseGeometricAltitude();
    bool ParseFigureOfMerit();
    bool ParseFieldExtensionIndicator();
    bool ParseNotImplement();

private:
    uint32_t fspec_;
    char* data_begin_;
    size_t data_block_len_;
    size_t record_len_;

    using action = std::function<bool (void)>;
    std::vector<action> actions_;
};

}//namespace core
//---------------------------------------------------------------------------
#endif //CORE_DATA_RECORD_H_
