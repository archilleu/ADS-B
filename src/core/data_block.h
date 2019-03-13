//---------------------------------------------------------------------------
#ifndef CORE_DATA_BLOCK_H_
#define CORE_DATA_BLOCK_H_
//---------------------------------------------------------------------------
#include <vector>
#include <cstdint>
//---------------------------------------------------------------------------
namespace core
{

class DataBlock
{
public:
    DataBlock(std::vector<char>&& data);

    bool Parse();

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
};


}//namespace core
//---------------------------------------------------------------------------
#endif //CORE_DATA_BLOCK_H_
