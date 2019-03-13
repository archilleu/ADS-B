//---------------------------------------------------------------------------
#include <cstring>
#include <arpa/inet.h>
#include <endian.h>
#include <iostream>
#include <cmath>
#include "../base/function.h"
#include "data_record.h"
//---------------------------------------------------------------------------
namespace core
{

//---------------------------------------------------------------------------
DataRecord::DataRecord(std::vector<uint8_t>&fspec, char* data_begin, size_t data_block_len)
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

        std::bind(&DataRecord::ParseLinkTechnology, this),
        std::bind(&DataRecord::ParseRollAngle, this),
        std::bind(&DataRecord::ParseFlightLevel, this),
        std::bind(&DataRecord::ParseAirSpeed, this),
        std::bind(&DataRecord::ParseTrueAirSpeed, this),
        std::bind(&DataRecord::ParseMagneticHeading, this),
        std::bind(&DataRecord::ParseBarometircVerticalRate , this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),

        std::bind(&DataRecord::ParseGeometricVerticalRate, this),
        std::bind(&DataRecord::ParseGrounVector, this),
        std::bind(&DataRecord::ParseRateOfTurn, this),
        std::bind(&DataRecord::ParseTargetIdentification, this),
        std::bind(&DataRecord::ParseVelocityAccuracy, this),
        std::bind(&DataRecord::ParseTimeOfDayAccuracy, this),
        std::bind(&DataRecord::ParseTargetStatus, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),

        std::bind(&DataRecord::ParseEmitterCategory, this),
        std::bind(&DataRecord::ParseMetReport, this),
        std::bind(&DataRecord::ParseIntermediateStateSelectedAltitude, this),
        std::bind(&DataRecord::ParseFinalStateSelectedAltitude, this),
        std::bind(&DataRecord::ParseTrajectoryIntent, this),
        std::bind(&DataRecord::ParseMode3_ACodeinOctalRepresentation, this),
        std::bind(&DataRecord::ParseSignalAmplitude, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),

        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this),
        std::bind(&DataRecord::ParseRE, this),
        std::bind(&DataRecord::ParseSP, this),
        std::bind(&DataRecord::ParseFieldExtensionIndicator, this)
    };

    InitLocalZeroTime();
}
//---------------------------------------------------------------------------
char* DataRecord::ParseRecord()
{
    for(size_t i=0; i<fspec_.size(); i++)
    {
        uint8_t sign = 0x80;
        for(size_t j=0; j<8; j++)
        {
            if(fspec_[i] & sign)
            {
                actions_[j]();
            }
            sign = static_cast<uint8_t>(sign >> 1);
        }
    }

    return nullptr;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseDataSourceIdentification()
{
    //数据长度不够，返回失败
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(reinterpret_cast<char*>(&buf), data_begin_, sizeof(buf));

    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "data source identification:" << str << std::endl;;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetReportDescriptor ()
{
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(reinterpret_cast<char*>(&buf), data_begin_, sizeof(buf));

    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "target report descriptor:" << str << std::endl;;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTimeOfDay()
{
    if((record_len_+3) > data_block_len_)
        return false;

    int elapsed = 0;
    memcpy(reinterpret_cast<char*>(& elapsed)+1, data_begin_, 3);
    elapsed = ntohl(elapsed) / LSB_TIME;
    time_t reported_time = zero_time_ + elapsed;

    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&reported_time) );
    std::cout << "time of day:" << tmp << std::endl;

    record_len_ += 3;
    data_begin_ += 3;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParsePositionWSG84()
{
    //必填
    if((record_len_+sizeof(uint32_t)*2) > data_block_len_)
        return false;

    uint32_t latitude = 0;
    uint32_t longitude = 0;
    memcpy(reinterpret_cast<char*>(&latitude), data_begin_, sizeof(uint32_t));
    memcpy(reinterpret_cast<char*>(&longitude), data_begin_+sizeof(uint32_t), sizeof(uint32_t));
    latitude = be32toh(latitude);
    longitude = be32toh(longitude);

    float lat = static_cast<float>(latitude) / LSB_WGS;
    float lng = static_cast<float>(longitude) / LSB_WGS;
    std::cout << "position wsg84: lat:" << lat << " lng:" << lng << std::endl;

    record_len_ += sizeof(uint32_t) * 2;
    data_begin_ += sizeof(uint32_t) * 2;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetAddress()
{
    //必填
    uint8_t buf[3];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "target address:" << str << std::endl;;
    
    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseGeometricAltitude()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t altitude = 0;
    memcpy(reinterpret_cast<char*>(&altitude), data_begin_, sizeof(uint16_t));
    altitude = be16toh(altitude);

    //单位ft
    float alt = static_cast<float>(altitude) / LSB_ALTITUDE;
    std::cout << "geometric altitude:" << alt << std::endl;
    
    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFigureOfMerit()
{
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "figure of merit :" << str << std::endl;
    
    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFieldExtensionIndicator()
{
    if(record_len_ == data_block_len_)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseLinkTechnology()
{
    uint8_t buf[1];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "ParseLinkTechnology:" << str << std::endl;;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseRollAngle()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t roll_angle;
    memcpy(reinterpret_cast<char*>(&roll_angle), data_begin_, sizeof(uint16_t));
    roll_angle = be16toh(roll_angle);
    float angle = static_cast<float>(roll_angle) / LSB_ROLL_ANGLE;
    std::cout << "ParseRollAngle:" << angle << std::endl;;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFlightLevel()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t flight_level;
    memcpy(reinterpret_cast<char*>(&flight_level), data_begin_, sizeof(uint16_t));
    flight_level= be16toh(flight_level);
    float level = static_cast<float>(flight_level) / LSB_FLIGHT_LEVEL;
    std::cout << "ParseFlightLevel:" << level << std::endl;;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseAirSpeed()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t air_speed;
    memcpy(reinterpret_cast<char*>(&air_speed), data_begin_, sizeof(uint16_t));
    air_speed = be16toh(air_speed);
    uint16_t mark = 0x8000;
    bool isMach = air_speed & mark;
    air_speed &= 0x7000;
    float speed = 0;
    if(isMach)//mach
    {
        speed = air_speed / LSB_MACH;
        std::cout << "ParseAirSpeed:" << speed << "(mach)" << std::endl;;
    }
    else//ias(nm/s)
    {
        speed = air_speed / LSB_IAS;
        std::cout << "ParseAirSpeed:" << speed << "(nm/s)" << std::endl;;
    }

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTrueAirSpeed()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t true_air_speed;
    memcpy(reinterpret_cast<char*>(&true_air_speed), data_begin_, sizeof(uint16_t));
    true_air_speed = be16toh(true_air_speed);
    std::cout << "ParseTrueAirSpeed:" << true_air_speed << std::endl;;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseMagneticHeading()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t magnetic_heading;
    memcpy(reinterpret_cast<char*>(&magnetic_heading), data_begin_, sizeof(uint16_t));
    magnetic_heading = be16toh(magnetic_heading);
    float heading = static_cast<float>(magnetic_heading) / LSB_MAGNETIC_HEADING;
    std::cout << "ParseMagneticHeading:" << heading << std::endl;;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseBarometircVerticalRate()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t barometirc_vertical_rate;
    memcpy(reinterpret_cast<char*>(&barometirc_vertical_rate), data_begin_, sizeof(uint16_t));
    barometirc_vertical_rate = be16toh(barometirc_vertical_rate);
    float rate = static_cast<float>(barometirc_vertical_rate) / LSB_BAROMETIRC_VERTICAL_RATE;
    std::cout << "ParseBarometircVerticalRate:" << rate << std::endl;;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseGeometricVerticalRate()
{
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t geometric_vertical_rate;
    memcpy(reinterpret_cast<char*>(&geometric_vertical_rate), data_begin_, sizeof(uint16_t));
    geometric_vertical_rate= be16toh(geometric_vertical_rate);
    float rate = static_cast<float>(geometric_vertical_rate) / LSB_GEOMETRIC_VERTICAL_RATE;
    std::cout << "ParseGeometricVerticalRate:" << rate << std::endl;;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseGrounVector()
{
    if((record_len_+sizeof(uint16_t)*2) > data_block_len_)
        return false;

    uint16_t ground_speed;
    memcpy(reinterpret_cast<char*>(&ground_speed), data_begin_, sizeof(uint16_t));
    ground_speed= be16toh(ground_speed);
    float rate = static_cast<float>(ground_speed) / LSB_GROUND_SPEED;

    uint16_t track_angle;
    memcpy(reinterpret_cast<char*>(&track_angle), data_begin_+sizeof(uint16_t), sizeof(uint16_t));
    track_angle = be16toh(track_angle);
    float angle = static_cast<float>(track_angle) / LSB_TRACK_ANGLE;


    std::cout << "ParseGrounVector:" << "speed:" << rate << " angle:" << angle << std::endl;;

    record_len_ += sizeof(uint16_t) * 2;
    data_begin_ += sizeof(uint16_t) * 2;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseRateOfTurn()
{
    uint8_t rate_of_turn;
    do
    {
        if((record_len_+sizeof(uint8_t)) > data_block_len_)
            return false;

        memcpy(reinterpret_cast<char*>(&rate_of_turn), data_begin_, sizeof(uint8_t));
        std::cout << "ParseRageOfTurn:" << rate_of_turn << ",";

        record_len_ += sizeof(uint8_t);
        data_begin_ += sizeof(uint8_t);
    }while(rate_of_turn & 0x01);
    std::cout << std::endl;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetIdentification()
{
    uint8_t buf[6];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(reinterpret_cast<char*>(&buf), data_begin_, sizeof(buf));

    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "ParseTargetIdentification:" << str << std::endl;;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseVelocityAccuracy()
{
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t accuracy;
    memcpy(reinterpret_cast<char*>(&accuracy), data_begin_, sizeof(uint8_t));
    std::cout << "ParseVelocityAccuracy:" << accuracy << std::endl;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTimeOfDayAccuracy()
{
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t day_accuracy;
    memcpy(reinterpret_cast<char*>(&day_accuracy), data_begin_, sizeof(uint8_t));
    float  accuracy = static_cast<float>(day_accuracy) / LSB_TIME_OF_DAY_ACCURACY;
    std::cout << "ParseTimeOfDayAccuracy:" << accuracy << std::endl;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetStatus()
{
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t status;
    memcpy(reinterpret_cast<char*>(&status), data_begin_, sizeof(uint8_t));
    std::cout << "ParseTargetStatus:" << status << std::endl;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseEmitterCategory()
{
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t category;
    memcpy(reinterpret_cast<char*>(&category), data_begin_, sizeof(uint8_t));
    std::cout << "ParseEmitterCategory:" << category << std::endl;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseMetReport()
{
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t report;
    memcpy(reinterpret_cast<char*>(&report), data_begin_, sizeof(uint8_t));
    std::cout << "ParseMetReport:" << report;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);

    uint8_t ws = 0x80;
    if(report & ws)
    {
        if((record_len_+sizeof(uint16_t)) > data_block_len_)
            return false;

        uint16_t wind_speed;
        memcpy(reinterpret_cast<char*>(&wind_speed), data_begin_, sizeof(uint16_t));
        wind_speed  = be16toh(wind_speed);
        std::cout << "wind speed:" << wind_speed << std::endl;;

        record_len_ += sizeof(uint16_t);
        data_begin_ += sizeof(uint16_t);
    }

    uint8_t wd = 0x40;
    if(report & wd)
    {
        if((record_len_+sizeof(uint16_t)) > data_block_len_)
            return false;

        uint16_t wind_direction;
        memcpy(reinterpret_cast<char*>(&wind_direction), data_begin_, sizeof(uint16_t));
        wind_direction = be16toh(wind_direction);
        std::cout << "wind direction :" << wind_direction << std::endl;;

        record_len_ += sizeof(uint16_t);
        data_begin_ += sizeof(uint16_t);
    }

    uint8_t tmp  = 0x20;
    if(report & tmp)
    {
        if((record_len_+sizeof(uint16_t)) > data_block_len_)
            return false;

        uint16_t temperature;
        memcpy(reinterpret_cast<char*>(&temperature), data_begin_, sizeof(uint16_t));
        temperature = be16toh(temperature);
        float temperature_f = static_cast<float>(temperature) / LSB_TEMPERATURE;
        std::cout << "temperature :" << temperature_f << std::endl;;

        record_len_ += sizeof(uint16_t);
        data_begin_ += sizeof(uint16_t);
    }

    uint8_t trb  = 0x10;
    if(report & trb)
    {
        if((record_len_+sizeof(uint8_t)) > data_block_len_)
            return false;

        uint16_t turbulence;
        memcpy(reinterpret_cast<char*>(&turbulence), data_begin_, sizeof(uint16_t));
        std::cout << "turbulence:" << turbulence << std::endl;;

        record_len_ += sizeof(uint8_t);
        data_begin_ += sizeof(uint8_t);
    }

    std::cout << std::endl;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseIntermediateStateSelectedAltitude()
{
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "ParseIntermediateStateSelectedAltitude:" << str << std::endl;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFinalStateSelectedAltitude()
{
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    std::string str = base::BinToString(buf, sizeof(buf));
    std::cout << "ParseFinalStateSelectedAltitude:" << str << std::endl;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTrajectoryIntent()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseMode3_ACodeinOctalRepresentation()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseSignalAmplitude()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseRE()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseSP()
{
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseNotImplement()
{
    return true;
}
//---------------------------------------------------------------------------
void DataRecord::InitLocalZeroTime()
{
    time_t now = time(NULL); 
    struct tm utc_zero;
    gmtime_r(&now, &utc_zero); 

    struct tm local_zero;
    localtime_r(&now, &local_zero); 
    local_zero.tm_hour = local_zero.tm_hour - utc_zero.tm_hour;  
    local_zero.tm_min = 0;  
    local_zero.tm_sec = 0;
    zero_time_  = mktime(&local_zero);
}
//---------------------------------------------------------------------------

}//namespace core
//---------------------------------------------------------------------------

