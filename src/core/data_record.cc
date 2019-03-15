//---------------------------------------------------------------------------
#include <cstring>
#include <arpa/inet.h>
#include <endian.h>
#include <iostream>
#include <cmath>
#include <assert.h>
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
                if(false == actions_[j+i*8]())
                    return nullptr;
            }
            sign = static_cast<uint8_t>(sign >> 1);
        }
    }

    return data_begin_;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseDataSourceIdentification()
{
    //010,2byte,定义看文档
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(reinterpret_cast<char*>(&buf), data_begin_, sizeof(buf));

    item_.data_source_identification_.sac_[0] = buf[0];
    item_.data_source_identification_.sic_[0] = buf[1];

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetReportDescriptor ()
{
    //040,2byte
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(reinterpret_cast<char*>(&buf), data_begin_, sizeof(buf));
    memcpy(reinterpret_cast<char*>(&item_.target_report_desc_), buf, sizeof(buf));

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTimeOfDay()
{
    //030,3byte,无符号整型
    if((record_len_+3) > data_block_len_)
        return false;

    int elapsed = 0;
    memcpy(reinterpret_cast<char*>(&elapsed)+1, data_begin_, 3);
    elapsed = ntohl(elapsed) / LSB_TIME;
    item_.time_of_day_ = zero_time_ + elapsed;

    record_len_ += 3;
    data_begin_ += 3;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParsePositionWSG84()
{
    //130,4byte+4byte,有符号整型
    if((record_len_+sizeof(int32_t)*2) > data_block_len_)
        return false;

    int32_t latitude = 0;
    int32_t longitude = 0;
    memcpy(reinterpret_cast<char*>(&latitude), data_begin_, sizeof(int32_t));
    memcpy(reinterpret_cast<char*>(&longitude), data_begin_+sizeof(int32_t), sizeof(int32_t));
    latitude = be32toh(latitude);
    longitude = be32toh(longitude);

    item_.latitude_ = static_cast<float>(latitude) / LSB_WGS;
    item_.longitude_ = static_cast<float>(longitude) / LSB_WGS;

    record_len_ += sizeof(int32_t) * 2;
    data_begin_ += sizeof(int32_t) * 2;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetAddress()
{
    //080,3byte
    uint8_t buf[3];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    memcpy(item_.target_address_, buf, sizeof(buf));
    
    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseGeometricAltitude()
{
    //140, 2byte,有符号整型
    if((record_len_+sizeof(int16_t)) > data_block_len_)
        return false;

    int16_t altitude = 0;
    memcpy(reinterpret_cast<char*>(&altitude), data_begin_, sizeof(int16_t));
    altitude = be16toh(altitude);

    //单位ft
    float alt = static_cast<float>(altitude) / LSB_ALTITUDE;
    item_.geometric_altitude_m_ = alt * FEET_PER_M;
    
    record_len_ += sizeof(int16_t);
    data_begin_ += sizeof(int16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFigureOfMerit()
{
    //090,2byte
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    memcpy(item_.figure_of_merit_, buf, sizeof(buf));
    
    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFieldExtensionIndicator()
{
    //分割标志位
    if(record_len_ == data_block_len_)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseLinkTechnology()
{
    //210,1byte
    uint8_t buf[1];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    item_.link_technology_[0] = buf[0];

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseRollAngle()
{
    //230,2byte,有符号整型
    if((record_len_+sizeof(int16_t)) > data_block_len_)
        return false;

    uint16_t roll_angle;
    memcpy(reinterpret_cast<char*>(&roll_angle), data_begin_, sizeof(int16_t));
    roll_angle = be16toh(roll_angle);
    item_.roll_angle_ = static_cast<float>(roll_angle) / LSB_ROLL_ANGLE;

    record_len_ += sizeof(int16_t);
    data_begin_ += sizeof(int16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFlightLevel()
{
    //145,2byte，有符号
    if((record_len_+sizeof(int16_t)) > data_block_len_)
        return false;

    int16_t flight_level;
    memcpy(reinterpret_cast<char*>(&flight_level), data_begin_, sizeof(int16_t));
    flight_level= be16toh(flight_level);
    item_.flight_level_ = static_cast<float>(flight_level) / LSB_FLIGHT_LEVEL;

    record_len_ += sizeof(int16_t);
    data_begin_ += sizeof(int16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseAirSpeed()
{
    //150,2byte
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t air_speed;
    memcpy(reinterpret_cast<char*>(&air_speed), data_begin_, sizeof(uint16_t));
    air_speed = be16toh(air_speed);
    air_speed &= 0x7000;
    if(air_speed & 0x8000)//mach
    {
        item_.air_speed_mach_ = air_speed / LSB_MACH;
    }
    else//ias(nm/s)
    {
        item_.air_speed_ias_ = air_speed / LSB_IAS;
    }

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTrueAirSpeed()
{
    //151,2byte,无符号
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t true_air_speed;
    memcpy(reinterpret_cast<char*>(&true_air_speed), data_begin_, sizeof(uint16_t));
    true_air_speed = be16toh(true_air_speed);
    item_.true_air_speed_ = true_air_speed * KNOT_PER_KMH;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseMagneticHeading()
{
    //152,2byte,无符号
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t magnetic_heading;
    memcpy(reinterpret_cast<char*>(&magnetic_heading), data_begin_, sizeof(uint16_t));
    magnetic_heading = be16toh(magnetic_heading);
    item_.magnetic_heading_ = static_cast<float>(magnetic_heading) / LSB_MAGNETIC_HEADING;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseBarometircVerticalRate()
{
    //155,2byte,无符号
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t barometirc_vertical_rate;
    memcpy(reinterpret_cast<char*>(&barometirc_vertical_rate), data_begin_, sizeof(uint16_t));
    barometirc_vertical_rate = be16toh(barometirc_vertical_rate);
    float rate = static_cast<float>(barometirc_vertical_rate) / LSB_BAROMETIRC_VERTICAL_RATE;
    item_.barometirc_vertical_rate_ = rate * FEET_PER_CM;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseGeometricVerticalRate()
{
    //157,2byte,无符号
    if((record_len_+sizeof(uint16_t)) > data_block_len_)
        return false;

    uint16_t geometric_vertical_rate;
    memcpy(reinterpret_cast<char*>(&geometric_vertical_rate), data_begin_, sizeof(uint16_t));
    geometric_vertical_rate = be16toh(geometric_vertical_rate);
    float rate = static_cast<float>(geometric_vertical_rate) / LSB_GEOMETRIC_VERTICAL_RATE;
    item_.geometric_vertical_rate_ = rate * FEET_PER_CM;

    record_len_ += sizeof(uint16_t);
    data_begin_ += sizeof(uint16_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseGrounVector()
{
    //160,2byte(有符号)+2byte(无符号)
    if((record_len_+sizeof(int16_t)+sizeof(uint16_t)) > data_block_len_)
        return false;

    int16_t ground_speed;
    memcpy(reinterpret_cast<char*>(&ground_speed), data_begin_, sizeof(int16_t));
    ground_speed = be16toh(ground_speed);
    item_.ground_speed_ = static_cast<float>(ground_speed) / LSB_GROUND_SPEED;

    uint16_t track_angle;
    memcpy(reinterpret_cast<char*>(&track_angle), data_begin_+sizeof(uint16_t), sizeof(uint16_t));
    track_angle = be16toh(track_angle);
    item_.track_angle_ = static_cast<float>(track_angle) / LSB_TRACK_ANGLE;

    record_len_ += sizeof(int16_t) * 2;
    data_begin_ += sizeof(uint16_t) * 2;
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseRateOfTurn()
{
    //165,1byte(+n),无符号
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
    assert(0);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetIdentification()
{
    //170,6byte无符号，表示8个字母,另外文档?
    uint8_t buf[6];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(reinterpret_cast<char*>(&buf), data_begin_, sizeof(buf));
    memcpy(reinterpret_cast<char*>(&item_.target_identification_), buf, sizeof(buf));

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseVelocityAccuracy()
{
    //095,1byte,另外文档？
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t accuracy;
    memcpy(reinterpret_cast<char*>(&accuracy), data_begin_, sizeof(uint8_t));
    item_.velocity_accuracy_[0] = accuracy;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTimeOfDayAccuracy()
{
    //032,1byte，无符号
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t day_accuracy;
    memcpy(reinterpret_cast<char*>(&day_accuracy), data_begin_, sizeof(uint8_t));
    item_.time_of_day_accuracy_ = static_cast<float>(day_accuracy) / LSB_TIME_OF_DAY_ACCURACY;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTargetStatus()
{
    //200,1byte
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t status;
    memcpy(reinterpret_cast<char*>(&status), data_begin_, sizeof(uint8_t));
    item_.target_status_ = status;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseEmitterCategory()
{
    //020
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t category;
    memcpy(reinterpret_cast<char*>(&category), data_begin_, sizeof(uint8_t));
    item_.emmiter_category_ = category;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseMetReport()
{
    //220,1byte+n
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t report;
    memcpy(reinterpret_cast<char*>(&report), data_begin_, sizeof(uint8_t));

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);

    uint8_t ws = 0x80;
    if(report & ws)
    {
        if((record_len_+sizeof(uint16_t)) > data_block_len_)
            return false;

        uint16_t wind_speed;
        memcpy(reinterpret_cast<char*>(&wind_speed), data_begin_, sizeof(uint16_t));
        item_.met_infomation_.wind_speed = be16toh(wind_speed) * KNOT_PER_KMH;

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
        item_.met_infomation_.wind_direction = be16toh(wind_direction);

        record_len_ += sizeof(uint16_t);
        data_begin_ += sizeof(uint16_t);
    }

    uint8_t tmp  = 0x20;
    if(report & tmp)
    {
        if((record_len_+sizeof(int16_t)) > data_block_len_)
            return false;

        int16_t temperature;
        memcpy(reinterpret_cast<char*>(&temperature), data_begin_, sizeof(int16_t));
        temperature = be16toh(temperature);
        item_.met_infomation_.temperature = static_cast<float>(temperature) / LSB_TEMPERATURE;

        record_len_ += sizeof(uint16_t);
        data_begin_ += sizeof(uint16_t);
    }

    uint8_t trb  = 0x10;
    if(report & trb)
    {
        if((record_len_+sizeof(uint8_t)) > data_block_len_)
            return false;

        uint8_t turbulence;
        memcpy(reinterpret_cast<char*>(&turbulence), data_begin_, sizeof(uint8_t));
        item_.met_infomation_.turbulence = turbulence;

        record_len_ += sizeof(uint8_t);
        data_begin_ += sizeof(uint8_t);
    }

    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseIntermediateStateSelectedAltitude()
{
    //146,2byte
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    uint8_t sas = 0x80;
    if(buf[0] & sas)
    {
        item_.ISS_altitude_.NoSourceInfomation = true;
        uint8_t mark = 0x60;
        uint8_t source = buf[0] & mark;
        if(0x00 == source)
        {
            item_.ISS_altitude_.type = Item::IntermediateStateSelectedAltitude::Unknown;
        }
        else if(0x01 == source)
        {
            item_.ISS_altitude_.type = Item::IntermediateStateSelectedAltitude::AircraftAltitude;
        }
        else if(0x10 == source)
        {
            item_.ISS_altitude_.type = Item::IntermediateStateSelectedAltitude::FCU_MSPSelectedAltitude;
        }
        else
        {
            item_.ISS_altitude_.type = Item::IntermediateStateSelectedAltitude::FMSSelectedAltitude;
        }
    }
    else
    {
        item_.ISS_altitude_.NoSourceInfomation = false;
    }
    //13/1bit
    buf[0] = buf[0] & 0x10;
    int16_t altitude = be16toh(*reinterpret_cast<int16_t*>(buf));
    item_.ISS_altitude_.altitude = static_cast<float>(altitude) / LSB_ISS_ALTITUDE * FEET_PER_M;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseFinalStateSelectedAltitude()
{
    //148,2byte
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    if(buf[0] & 0x80)
    {
        item_.FSS_altitude_.manage_vertical_mode = true;
    }

    if(buf[0] & 0x40)
    {
        item_.FSS_altitude_.altitude_hold_mode = true;
    }
    if(buf[0] & 0x20)
    {
        item_.FSS_altitude_.aproach_mode = true;
    }

    //13/1
    buf[0] = buf[0] & 0x10;
    int16_t altitude = be16toh(*reinterpret_cast<int16_t*>(buf));
    item_.FSS_altitude_.altitude = static_cast<float>(altitude) / LSB_FSS_ALTITUDE * FEET_PER_M;

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseTrajectoryIntent()
{
    //110
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t intent;
    memcpy(reinterpret_cast<char*>(&intent), data_begin_, sizeof(uint8_t));
    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);

    if(intent & 0x80)
    {
        assert(intent & 0x01);

        if((record_len_+sizeof(uint8_t)) > data_block_len_)
            return false;

        uint8_t intent_status;
        memcpy(reinterpret_cast<char*>(&intent_status), data_begin_, sizeof(uint8_t));

        if(intent_status & 0x80)
        {
            item_.trajectory_intent_.intent_status.nav = true;
        }
        if(intent_status & 0x40)
        {
            item_.trajectory_intent_.intent_status.nvb = true;
        }

        record_len_ += sizeof(uint8_t);
        data_begin_ += sizeof(uint8_t);

    }
    if(intent & 0x40)
    {
        assert(intent & 0x01);
        assert(0);
    }

    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseMode3_ACodeinOctalRepresentation()
{
    //070
    uint8_t buf[2];
    if((record_len_+sizeof(buf)) > data_block_len_)
        return false;

    memcpy(buf, data_begin_, sizeof(buf));
    memcpy(item_.mode3_, buf, sizeof(buf));

    record_len_ += sizeof(buf);
    data_begin_ += sizeof(buf);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseSignalAmplitude()
{
    //131
    if((record_len_+sizeof(uint8_t)) > data_block_len_)
        return false;

    uint8_t amplitude;
    memcpy(reinterpret_cast<char*>(&amplitude), data_begin_, sizeof(uint8_t));
    item_.signal_ampltitude_ = amplitude;

    record_len_ += sizeof(uint8_t);
    data_begin_ += sizeof(uint8_t);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseRE()
{
    //RE
    assert(0);
    return true;
}
//---------------------------------------------------------------------------
bool DataRecord::ParseSP()
{
    //SP
    assert(0);
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

