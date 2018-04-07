#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WBDevice.h"

const char *g_Topics[] =
{
	"error",
	"switch",
	"alarm",
	"pushbutton",
	"range",
	"rgb",
	"text",
	"value",
	"temperature",
	"rel_humidity",
	"atmospheric_pressure",
	"sound_level",
	"PrecipitationRate", //(rainfall rate)	rainfall	mm per hour	float
	"WindSpeed", //	wind_speed	m / s	float
	"PowerPower", //	watt	float
	"PowerConsumption", //	power_consumption	kWh	float
	"VoltageVoltage", //	volts	float
	"WaterFlow", //	water_flow	m ^ 3 / hour	float
	"WaterTotal", // consumption	water_consumption	m ^ 3	float
	"Resistance", //	resistance	Ohm	float
	"concentration", //	concentration	ppm	float(unsigned)
  "lux"
	"",
};


CMqttControl::CMqttControl(const char *name, ControlType type, bool readonly, const char *value)
:Name(name), Type(type), Readonly(readonly)
{
  if (value)
    Value = value;
}

CMqttControl::CMqttControl(CMqttDevice *parent, const char *name, ControlType type, bool readonly, const char *value)
:Parent(parent), Name(name), Type(type), Readonly(readonly)
{
  if (value)
    Value = value;
}

void CMqttControl::SetType(const char *name, ControlType type, bool readonly, const char *value)
{
	Name = name;
	Type = type;
	Readonly = readonly;
  if (value)
    Value = value;
}

void CMqttControl::Create(int order)
{
  char buffer[20];
  Parent->publish(String("/controls/")+Name+"/meta/type", g_Topics[Type]);
  Parent->publish(String("/controls/")+Name+"/meta/order", itoa(order, buffer, 10));

  if (Readonly)
    Parent->publish(String("/controls/")+Name+"/meta/readonly", "1");

  Publish(false);
}

void CMqttControl::Publish(bool onlyChanged)
{
  if (!Changed && onlyChanged)
    return;

  Parent->publish((String)"/controls/"+Name, Value);
  Changed=false;
}

void CMqttControl::Subscribe(bool onlyOn)
{
	if (onlyOn)
		Parent->subscribe((String)"/controls/"+Name+"/on");
	else
		Parent->subscribe((String)"/controls/"+Name);
}

void CMqttControl::SetValue(const char *value)
{
  if (Value!=value)
  {
    Changed=true;
    Value=value;
  }
}

String CMqttControl::GetValue()
{
	return Value;
}

bool CMqttControl::compareTopic(String path)
{
	if (path == Parent->getTopic()+"/controls/"+Name+"/on") return true;
	if (path == Parent->getTopic()+"/controls/"+Name) return true;
	return false;
}



CMqttDevice::CMqttDevice(PubSubClient *client, const char* name, const char* description, int controlCount, CMqttControl *controls)
:Client(client), Name(name), Description(description), ControlCount(controlCount), Controls(controls)
{
  for (int i=0;i<ControlCount;i++)
    Controls[i].SetParent(this);
}

void CMqttDevice::SetControls(CMqttControl *controls)
{
  Controls = controls;
}

String CMqttDevice::getTopic()
{
  return String("/devices/")+Name;
}

void CMqttDevice::Create()
{
  publish("/meta/name", Description);
  for (int i=0;i<ControlCount;i++)
    Controls[i].Create(i+1);
}

void CMqttDevice::publish(const char *path, const char *value)
{
  Client->publish((getTopic()+path).c_str(), value, true);
}


void CMqttDevice::publish(String path, String value)
{
  Client->publish((getTopic()+path).c_str(), value.c_str(), true);
}

void CMqttDevice::subscribe(const char *path)
{
	Client->subscribe((getTopic()+path).c_str());

}

void CMqttDevice::subscribe(String path)
{
	Client->subscribe((getTopic()+path).c_str());
}


void CMqttDevice::Set(int id, const char *value)
{
  Controls[id].SetValue(value);
}

void CMqttDevice::Set(const char *id, const char *value)
{
	for (int i=0;i<ControlCount;i++)
		if (!strcmp(id, Controls[i].Name))
    	Controls[i].SetValue(value);

}

void CMqttDevice::Set(const char *id, int value)
{
	char buffer[20];
  Set(id, itoa(value, buffer, 10));
}

void CMqttDevice::Set(int id, int value)
{
  char buffer[20];
  Controls[id].SetValue(itoa(value, buffer, 10));
}

int CMqttDevice::GetInt(int id)
{
	return atoi(Controls[id].Value.c_str());
}

float CMqttDevice::GetFloat(int id)
{
	return atof(Controls[id].Value.c_str());
}

String CMqttDevice::GetStr(int id)
{
	return Controls[id].Value;
}


void CMqttDevice::Publish(bool onlyChanged)
{
  for (int i=0;i<ControlCount;i++)
    Controls[i].Publish(onlyChanged);
}

void CMqttDevice::Subscribe(bool onlyOn)
{
	for (int i=0;i<ControlCount;i++)
    Controls[i].Subscribe(onlyOn);
}

CMqttControl *CMqttDevice::GetControlFromTopic(String path)
{
	for (int i=0;i<ControlCount;i++)
		if (Controls[i].compareTopic(path))
			return Controls+i;

	return NULL;
}
