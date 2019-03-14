//---------------------------------------------------------------------------
#ifndef CORE_DATA_BLOCK_H_
#define CORE_DATA_BLOCK_H_
//---------------------------------------------------------------------------
#include <vector>
#include <cstdint>
#include "data_record.h"
//---------------------------------------------------------------------------
namespace core
{

class DataBlock
{
public:
    DataBlock(std::vector<char>&& data);

    bool Parse();

    const std::vector<DataRecord>& records() { return records_; }

private:
    bool VerifyCAT();
    bool GetDataRecordLen();

    std::vector<uint8_t> GetFSPEC();
    bool CheckFSPECEnd(char fspec);

private:
    const static uint8_t CAT_DATA_BLOCK = 0x15;

private:
    std::vector<char> data_;
    uint16_t record_len_;
    char* data_begin_;
    char* data_end_;

    std::vector<DataRecord> records_;
};

}//namespace core
//---------------------------------------------------------------------------
#endif //CORE_DATA_BLOCK_H_
