//---------------------------------------------------------------------------
#include <string.h>
#include "../base/json_writer.h"
#include "../base/value.h"
#include "../base/function.h"
#include "item.h"
//---------------------------------------------------------------------------
namespace core
{

//---------------------------------------------------------------------------
Item::Item()
{
    bzero(this, sizeof(Item));
}
//---------------------------------------------------------------------------
std::string Item::ToString() const
{
    json::Value value(json::Value::OBJECT);
    value.PairAdd("data_source_identification",
            std::string("sac:")+base::BinToString(data_source_identification_.sac_, 1) +
            " sic:"+base::BinToString(data_source_identification_.sic_, 1));

    value["target_report_desc"] = base::BinToString(target_report_desc_, sizeof(target_report_desc_));

    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&time_of_day_));
    value["time_of_day"] =  tmp;

    value["latitude"] = latitude_;
    value["longitude"] = longitude_;

    value["target_address"] = base::BinToString(target_address_, sizeof(target_address_));

    value["geometric_altitude(m)"] = geometric_altitude_m_;

    value["figure_of_merit"] = base::BinToString(figure_of_merit_, sizeof(figure_of_merit_));

    value["link_technology"] = base::BinToString(link_technology_, sizeof(link_technology_));

    value["roll_angel(degree)"] = roll_angle_;

    value["flight_level(FL)"] = flight_level_;

    value["air_speed_ias(NM/s)"] = air_speed_ias_;
    value["air_speed_mach(mach)"] = air_speed_mach_;

    value["true_air_speed(km/h)"] = true_air_speed_;

    value["magnetic_heading(°)"] = magnetic_heading_;

    value["barometirc_vertical_rate(cm/min)"] = barometirc_vertical_rate_;
    value["geometric_vertical_rate(cm/min)"] = geometric_vertical_rate_;

    value["ground_speed(NM/s)"] = ground_speed_;
    value["track_angle(°)"] = track_angle_;

    value["target_identification"] = base::BinToString(target_identification_, sizeof(target_identification_));

    value["velocity_accuracy"] = base::BinToString(velocity_accuracy_, sizeof(velocity_accuracy_));

    value["time_of_day_accuracy"] = time_of_day_accuracy_;

    value["target_status"] = target_status_;

    value["emmiter_category"] = emmiter_category_;

    json::Value met_infomation(json::Value::OBJECT);
    met_infomation["wind_speed(km/h)"] = met_infomation_.wind_speed;
    met_infomation["wind_direction(°)"] = met_infomation_.wind_direction;
    met_infomation["temperature(℃ )"] = met_infomation_.temperature;
    met_infomation["turbulence"] = met_infomation_.turbulence;
    value["met_infomation"] = met_infomation;

    json::Value ISS_altitude(json::Value::OBJECT);
    ISS_altitude["NoSourceInfomation"] = ISS_altitude_.NoSourceInfomation;
    ISS_altitude["type"] = ISS_altitude_.type;
    ISS_altitude["altitude"] = ISS_altitude_.altitude;
    value["ISS_altitude"] = ISS_altitude;

    json::Value FSS_altitude(json::Value::OBJECT);
    FSS_altitude["manage_vertical_mode"] = FSS_altitude_.aproach_mode;
    FSS_altitude["altitude_hold_mode"] = FSS_altitude_.altitude_hold_mode;
    FSS_altitude["aproach_mode"] = FSS_altitude_.aproach_mode;
    value["FSS_altitude"] = FSS_altitude;

    json::Value trajectory_intent(json::Value::OBJECT);
    json::Value trajectory_intent_status(json::Value::OBJECT);
    trajectory_intent_status["nav"] = trajectory_intent_.intent_status.nav;
    trajectory_intent_status["nvb"] = trajectory_intent_.intent_status.nvb;
    trajectory_intent["intent_status"] = trajectory_intent_status;
    value["trajectory_intent"] = trajectory_intent;

    value["Mode3_ACodeinOctalRepresentation"] = base::BinToString(mode3_, sizeof(mode3_));;

    value["signal_ampltitude"] = signal_ampltitude_;

    std::string result = json::JsonWriter(value).ToString(true);
    return result;
}
//---------------------------------------------------------------------------

}//namespace core
//---------------------------------------------------------------------------

