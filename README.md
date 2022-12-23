# Oh-Weather

## Overview
Oh-Weather is a project built on Lilygo ESP-32 ( esp-32 with a small screen on
top). Oh-Weather will fetch data from openweathermap and show the weather data with
images exploiting lvgl library.

## Configure
Oh-Weather can be configured by using config/config.json.
```json
{
    "sysConfig": {
        "pollingInterval": "300000",
        "developmentMode": 1,
        "timeZone": "CET-1CEST,M3.5.0,M10.5.0/3",
        "wifiSSID": "WIFI-SSID",
        "wifiPassphrase": "WIFI-PASSWORD",
        "apSSID": "Oh-Weather",
        "apPassphrase": "12345678"
    },
    "weatherRequest": {
        "requestPlatform": "openweathermap",
        "apiEndpoint": "https://api.openweathermap.org/data/2.5/weather",
        "requestApiKey": "OPENWEATHERMAP-API-KEY",
        "cityName": "Milan",
        "cityId": "",
        "zipCode": "",
        "stateCode": "",
        "countryCode": "",
        "coordinates" : {"latitude": "", "longitude": ""}
    }
}
```
The config file is divided into two main sections: ***sysConfig*** and ***weatherRequest***

- **sysConfig**:
  - **pollingInterval**: polling interval in milliseconds.
  - **developmentMode**: show debugging output.
  - **timeZone**: time zone represented in POSIX TZ environment variable format. More details on other [timezones](https://ftp.fau.de/aminet/util/time/tzinfo.txt).
  - **wifiSSID**: WiFi SSID to connect to.
  - **wifiPassphrase**: WiFi passphrase.
  - **apSSID**: access point SSID to be generated for boostrapping. A web server (on-going work) will pop up and an user can use the web server to configure the board.
  - **apPassphrase**: access point passphrase
- **weatherRequest**:
  - **requestPlatform**: the platform that the board will fetch data from. Currently, only openweathermap is supported.
  - **apiEndpoint**: API end point.
  - **requestApiKey**: API key retrieved from the platform.
  - **cityName**: the city name that the data should be requested.
  - **cityId**:  city id that the data should be requested.
  - **zipCode**: the zip code that the data should be requested.
  - **stateCode**: the state code that the data should be requested.
  - **countryCode**: the country code that the data should be requested.
  - **coordinates**: the coordinates that the data should be requested.

Note that Oh-Weather will check *cityName*, *cityId*, *zipCode*, *stateCode*, *countryCode*, and *coordinates* in sequence, it will stop if it finds the correct info.


## Status
Currently, the project is on-going. The missing part is to use a web server to 
configure the board.


