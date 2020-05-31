/*This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#ifndef CONFIG_WIFI_HEADER
#define CONFIG_WIFI_HEADER

// nom du réseaux wifi
#define WIFI_SSID "iPhone"
// mot de passe du wifi
#define WIFI_PWD "testmqtt"

// ip du brocker MQTT
#define MQTT_IP IPAddress(20, 188, 44, 39)
// port du brocker MQTT
#define MQTT_PORT 1880

// nom d'utilisateur MQTT
#define MQTT_NAME ""
// mot de passe de l'utilisateur mqtt
#define MQTT_PWD ""

// clé API google pour géocalisation
#define API_KEY "AIzaSyA0E-jxZROG2GbdiQHzmCUS9cU-x-Sh0KA"

#endif // CONFIG_WIFI_HEADER