/* -*- mode: c -*- */

/*
   Antiflood Copyright (C) 2018 Alexey <Inverse> Shumeiko

   This file is part of Antiflood project.

   This firmware is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This firmware is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the firmware. If not, see <http://www.gnu.org/licenses/>.

   For more details see LICENSE file.
*/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>
#include "debug.h"



/*
 * Port mapping.
 */

#define WIFIRX    0                         /**< WiFi-shield RX */
#define WIFITX    1                         /**< WiFi-shield TX */
#define WIFIEN    2                         /**< enable WiFi-shield (active high) */
#define WIFIRS    3                         /**< reset WiFi-shield (active low) */

#define SW_RX     4                         /**< software serial rx (debug printout) */
#define SW_TX     5                         /**< software serial tx (debug printout) */

#define VCLOSE    6                         /**< close valves */
#define VOPEN     7                         /**< open valves */

#define LED0      8                         /**< LED 0 */
#define LED1      9                         /**< LED 1 */
#define LED2      10                        /**< LED 2 */
#define LED3      11                        /**< LED 3 */
#define LED4      12                        /**< LED 4 */
#define LED5      13                        /**< LED 5 */

#define PROBE0    A0                        /**< probe 0 */
#define PROBE1    A1                        /**< probe 1 */
#define PROBE2    A2                        /**< probe 2 */
#define PROBE3    A3                        /**< probe 3 */
#define PROBE4    A4                        /**< probe 4 */
#define PROBE5    A5                        /**< probe 5 */

#define VCONSC    A6                        /**< valve supply current */
#define VFOPST    A7                        /**< valve's forced state */



/*
 * Configures.
 */

/* Communication settings. */
#define APP_DEFAULT_IP 192, 168, 0, 111     /**< application default IP address */
#define APP_DEFAULT_IP_S "192.168.0.111"    /**< application default IP address string */
#define APP_DEFAULT_PORT 80                 /**< application default port */

/* Power save settings. */
#define USE_POWER_SAVE                      /**< use power saving mode between loop cycles */

/* Network interface settings. */
#define WIFI_STATION                        /**< wireless interface mode */
#define WIFI_DEFAULT_SSID ">)"              /**< default network identifier */
#define WIFI_DEFAULT_PASS "00000000"        /**< default network access password */
#define WIFI_DEFAULT_CHAN 10                /**< default network channel number */
#define WIFI_DEFAULT_SECU ENC_TYPE_WPA2_PSK /**< default authentication type */



#endif  /* __CONFIG_H__ */
