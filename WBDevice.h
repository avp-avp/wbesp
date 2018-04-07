#ifndef __WB_DEVICE_H
#define __WB_DEVICE_H

#include <PubSubClient.h>

class CMqttDevice;

struct CMqttControl
{
  enum ControlType
      {
        Error=0,
        Switch,  //0 or 1
        Alarm, //
        PushButton, // 1
        Range, // 0..255 [TODO] - max value
        Rgb,
        Text,
        Generic,
        Temperature, //	temperature	°C	float
        RelativeHumidity, //	rel_humidity	%, RH	float, 0 - 100
        AtmosphericPressure, //	atmospheric_pressure	millibar(100 Pa)	float
        SoundLevel,
        PrecipitationRate, //(rainfall rate)	rainfall	mm per hour	float
        WindSpeed, //	wind_speed	m / s	float
        PowerPower, //	watt	float
        PowerConsumption, //	power_consumption	kWh	float
        VoltageVoltage, //	volts	float
        WaterFlow, //	water_flow	m ^ 3 / hour	float
        WaterTotal, // consumption	water_consumption	m ^ 3	float
        Resistance, //	resistance	Ohm	float
        GasConcentration, //	concentration	ppm	float(unsigned),
        Lux
      };

    const char *Name;
    ControlType Type;
    bool Readonly;
    String Value;

    CMqttControl();
    CMqttControl(CMqttDevice *parent, const char *topic, ControlType type, bool Readonly=false, const char *value=NULL);
    CMqttControl(const char *topic, ControlType type, bool Readonly=false, const char *value=NULL);
    void SetType(const char *topic, ControlType type, bool Readonly=false, const char *value=NULL);
    bool isChanged(){return Changed;};
    void Publish(bool onlyChanged=true);
    void Subscribe(bool onlyOn=true);
    void SetValue(const char *value);
    String GetValue();
    void SetParent(CMqttDevice *parent){Parent=parent;};
    void Create(int order);
    bool compareTopic(String path);

    private:
      CMqttDevice *Parent;
      bool Changed;
};

class CMqttDevice
{
  friend class CMqttControl;
  PubSubClient *Client;
  const char *Name, *Description;
  int ControlCount;
  CMqttControl *Controls;

public:
  String getTopic();
  CMqttDevice(PubSubClient *client, const char* name, const char* description, int ControlCount, CMqttControl *controls=NULL);
  void SetControls(CMqttControl *controls);
  void Create();
  void Set(int id, const char *calue);
  void Set(int id, int value);
  void Set(const char *id, const char *value);
  void Set(const char *id, int value);
  int GetInt(int id);
  float GetFloat(int id);
  String GetStr(int id);
  void Publish(bool onlyChanged=true);
  void Subscribe(bool onlyOn=true);
  CMqttControl *GetControlFromTopic(String path);

  void publish(const char *path, const char *value);
  void publish(String path, String value);
  void subscribe(const char *path);
  void subscribe(String path);
};

#endif
