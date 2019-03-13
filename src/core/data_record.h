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
    DataRecord(std::vector<uint8_t>& fspec, char* data_begin, size_t data_block_len);
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

    bool ParseLinkTechnology();
    bool ParseRollAngle();
    bool ParseFlightLevel();
    bool ParseAirSpeed();
    bool ParseTrueAirSpeed();
    bool ParseMagneticHeading();
    bool ParseBarometircVerticalRate();

    bool ParseGeometricVerticalRate();
    bool ParseGrounVector();
    bool ParseRateOfTurn();
    bool ParseTargetIdentification();
    bool ParseVelocityAccuracy();
    bool ParseTimeOfDayAccuracy();
    bool ParseTargetStatus();

    bool ParseEmitterCategory();
    bool ParseMetReport();
    bool ParseIntermediateStateSelectedAltitude();
    bool ParseFinalStateSelectedAltitude();
    bool ParseTrajectoryIntent();
    bool ParseMode3_ACodeinOctalRepresentation();
    bool ParseSignalAmplitude();

    bool ParseRE();
    bool ParseSP();

    bool ParseNotImplement();

private:
    void InitLocalZeroTime();

private:
    std::vector<uint8_t> fspec_;
    char* data_begin_;
    size_t data_block_len_;
    size_t record_len_;

    using action = std::function<bool (void)>;
    std::vector<action> actions_;

    time_t zero_time_;
    const static uint8_t LSB_TIME = 128;
    constexpr const static float LSB_WGS = 186413.516f;
    constexpr const static float LSB_ALTITUDE = (1.0f/6.25f);
    const static uint8_t LSB_ROLL_ANGLE = 100;
    const static uint8_t LSB_FLIGHT_LEVEL = 4;
    const static uint16_t LSB_IAS = 16384;
    const static uint16_t LSB_MACH = 1000;
    constexpr const static float LSB_MAGNETIC_HEADING = (65536.0f/360.0f);
    constexpr const static float LSB_BAROMETIRC_VERTICAL_RATE = (1.0f/6.25f);
    constexpr const static float LSB_GEOMETRIC_VERTICAL_RATE = (1.0f/6.25f);
    const static uint16_t LSB_GROUND_SPEED = 16384;
    constexpr const static float LSB_TRACK_ANGLE = (65536.0f/360.0f);
    const static uint16_t LSB_RATE_OF_TURN = 4;
    const static uint16_t LSB_TIME_OF_DAY_ACCURACY = 256;
    const static uint16_t LSB_TEMPERATURE = 4;
};

}//namespace core
//---------------------------------------------------------------------------
#endif //CORE_DATA_RECORD_H_
