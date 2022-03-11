#include <ESP8266WebServer.h>
#include <Arduino.h>
#pragma once
class base
{
	public:
		base (ESP8266WebServer* server);

		void Render (void);
		void GetAjaxValues (void);
		void SetCallback_submit (void (*callback)(void));
		void Set_calibrated (String value);
		String Get_calibrated ();
		void Set_turbo (String value);
		String Get_turbo ();
		void Set_speed (String value);
		String Get_speed ();
		void Set_timer (String value);
		String Get_timer ();
		void Set_version (String value);
		String Get_version ();
		void Set_tempIst (String value);
		String Get_tempIst ();
		void Set_heating (String value);
		String Get_heating ();
		void Set_tempSoll (String value);
		String Get_tempSoll ();
		void Set_state (String value);
		String Get_state ();
	private:
		void Submit_Callback(void);
		void(*submit_UserCallback)(void);
		ESP8266WebServer* server;

		String calibrated;
		String turbo;
		String speed;
		String timer;
		String version;
		String tempIst;
		String heating;
		String tempSoll;
		String state;
};