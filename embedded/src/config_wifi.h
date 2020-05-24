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
#define WIFI_SSID ""
// mot de passe du wifi
#define WIFI_PWD ""

// ip du brocker MQTT
#define MQTT_IP IPAddress(0, 0, 0, 0)
// port du brocker MQTT
#define MQTT_PORT 1880

// nom d'utilisateur MQTT
#define MQTT_NAME ""
// mot de passe de l'utilisateur mqtt
#define MQTT_PWD ""

// clé API google pour géocalisation
#define API_KEY "AIzaSyA1xl75rhbHZKvADw28rSl8MuNc6snGOBU"

#endif // CONFIG_WIFI_HEADER