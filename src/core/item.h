//---------------------------------------------------------------------------
#ifndef CORE_ITEM_H_
#define CORE_ITEM_H_
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include <cstdint>
#include <time.h>
//---------------------------------------------------------------------------
namespace core
{

class Item
{
public:
    Item();

    std::string ToString() const;

public:
    struct DataSourceIdentification
    {
        uint8_t sac_[1];
        uint8_t sic_[1];
    }data_source_identification_;

    uint8_t target_report_desc_[2];

    time_t time_of_day_;

    float latitude_;
    float longitude_;

    uint8_t target_address_[3];

    float geometric_altitude_m_;

    uint8_t figure_of_merit_[2];

    uint8_t link_technology_[1];

    float roll_angle_;  //单位degree -180->180

    float flight_level_; //单位FL

    //单位分为ias(NM/s)和mach计量
    float air_speed_ias_;
    float air_speed_mach_;

    float true_air_speed_;//单位km/h

    float magnetic_heading_; //单位°

    float barometirc_vertical_rate_; //单位cm/min

    float geometric_vertical_rate_; //单位cm/min

    float ground_speed_; //单位NM/s
    float track_angle_; //单位°

    //std::vector<uint8_t> rate_of_trun_;    
    
    uint8_t target_identification_[6];

    uint8_t velocity_accuracy_[1];

    float time_of_day_accuracy_;

    uint8_t target_status_;

    uint8_t emmiter_category_;

    struct MetInformation
    {
        float wind_speed; //单位 km/h
        uint16_t wind_direction; //单位 °
        float temperature; //单位℃
        uint8_t turbulence;
    }met_infomation_;

    struct IntermediateStateSelectedAltitude
    {
        bool NoSourceInfomation;
        enum
        {
            Unknown,
            AircraftAltitude,
            FCU_MSPSelectedAltitude,
            FMSSelectedAltitude
        }type;
        float altitude; //单位m 
    }ISS_altitude_;

    struct FinalStateSelectedAltitude
    {
        bool manage_vertical_mode;
        bool altitude_hold_mode;
        bool aproach_mode;
        float  altitude; // 单位m
    }FSS_altitude_;

    struct TrajectoryIntent
    {
        struct
        {
            bool nav;
            bool nvb;
        }intent_status;
        
        struct
        {
        }intent_data;
    }trajectory_intent_;

    uint8_t mode3_[2]; 

    uint8_t signal_ampltitude_;
};

}//namespace core
//---------------------------------------------------------------------------
#endif //CORE_ITEM_H_
