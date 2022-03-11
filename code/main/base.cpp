#include "base.hpp"
#include <ArduinoJson.h>

StaticJsonDocument < 300 > base_doc;
const char base_text[] = "﻿<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\">\n\
\n\
<head>\n\
	<link rel=\"stylesheet\" style=\"text/css\">\n\
<meta content=\"de\" http-equiv=\"Content-Language\" />\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\" />\n\
<title>Embedded Template</title>\n\
<style type=\"text/css\">\n\
	.button {\n\
	  display: inline-block;\n\
	  padding: 15px 20px;\n\
	  font-size: 20px;\n\
	  cursor: pointer;\n\
	  text-align: center;\n\
	  text-decoration: none;\n\
	  outline: none;\n\
	  color: #fff;\n\
	  background-color: silver;\n\
	  border: none;\n\
	  border-radius: 10px;\n\
	  box-shadow: 0 9px black;\n\
	}\n\
</style>\n\
\n\
<body>\n\
	<h1>Embedded Template</h1>\n\
	<table style=\"width: 100%\">\n\
		<tr>\n\
			<td><strong>From c++: </strong></td>\n\
			<td><strong id=\"dynamicVariable\"></strong></td>\n\
		</tr>\n\
		<tr>\n\
			<td><strong>To c++: </strong></td>\n\
			<td><input type=\"text\" id=\"dynamicVariable2\"onchange=\"updateDynamicVariable2()\"></strong></td>\n\
			<td>\n\
				<form action=\"submit\" method=\"post\" >\n\
					<button class=\"button\" id=\"submitBtn\" type=\"submit\" name=\"action\" value=\"\" >übernehmen</button>\n\
				</form>\n\
			</td>\n\
		</tr>\n\
	</table>\n\
</body>\n\
</html>\n\
\n\
<script>\n\
	var values =\n\
	{\n\
		\"dynamicVariable\" : \"dynamic\",\n\
		\"dynamicVariable2\" : \"dynamic\"\n\
	}\n\
\n\
	document.getElementById(\"dynamicVariable\").innerHTML = values.dynamicVariable\n\
	interval = setInterval(updateValues, 1000)\n\
\n\
	function updateDynamicVariable2()\n\
	{\n\
		values.dynamicVariable2 = document.getElementById(\"dynamicVariable2\").value\n\
		document.getElementById(\"submitBtn\").value = JSON.stringify(values)\n\
	}\n\
\n\
	function updateValues()\n\
	{\n\
		var xhttp = new XMLHttpRequest();\n\
		xhttp.onreadystatechange = function(){\n\
			pairs = this.responseText.split(\";\");\n\
			for(let i = 0; i < pairs.length; i++)\n\
			{\n\
				pair = pairs[i].split(':');\n\
				values[pair[0]] = pair[1];\n\
				document.getElementById(pair[0]).innerHTML = pair[1]\n\
			}\n\
		}\n\
		xhttp.open(\"GET\", \"getValues\", true);\n\
		xhttp.send();\n\
	}\n\
\n\
</script>\n\
";

base::base(ESP8266WebServer* server)
{
	this->server = server;
	this->submit_UserCallback = (void(*)()) NULL;
	this->server->on("/submit", std::bind( & base::Submit_Callback, this));
	this->server->on("/getValues", std::bind( & base::GetAjaxValues, this));
}
void base::Submit_Callback(void)
{
	String jsonstring = this->server->arg("action");
	DeserializationError error = deserializeJson(base_doc, jsonstring);
	Serial.println(jsonstring);
	JsonObject obj = base_doc.as < JsonObject > ();

	if (error)
	{
	Serial.print(F("deserializeJson() failed: "));
	Serial.println(error.f_str());
	}
	else
	{
		this->dynamicVariable2 = obj["dynamicVariable2"].as < String > ();
		this->dynamicVariable = obj["dynamicVariable"].as < String > ();

	}
	if (NULL != this->submit_UserCallback)
	{
		this->submit_UserCallback();
	}
}

void base::SetCallback_submit (void (*callback)(void))
{
	this->submit_UserCallback = callback;
}

void base::Set_dynamicVariable2 (String value)
{
	this->dynamicVariable2 = value;
}

String base::Get_dynamicVariable2 ( void )
{
	return this->dynamicVariable2;
}
void base::Set_dynamicVariable (String value)
{
	this->dynamicVariable = value;
}

String base::Get_dynamicVariable ( void )
{
	return this->dynamicVariable;
}
void base::Render( void )
{
	this->server->send( 200, base_text);
}
void base::GetAjaxValues( void )
{
	String message = "dynamicVariable2:" + this->dynamicVariable2 + ";dynamicVariable:" + this->dynamicVariable;
	this->server->send(200, "text/plain", message);
}
