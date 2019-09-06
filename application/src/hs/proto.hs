{-
   Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko

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
-}

-- device's protocol commands, states, flows, etc.
module Proto where

import Data.Maybe (fromJust)
import Data.Tuple (swap)
import Data.Word (Word8)



data Cls = CEcho | CInfo | CRequest | CResponse | CRFU
         deriving (Eq)

instance Enum Cls where
    fromEnum = fromJust . flip lookup cls_table
    toEnum = fromJust . flip lookup (map swap cls_table)
cls_table = [(CEcho, 0),
             (CInfo, 1),
             (CRequest, 2),
             (CResponse, 3),
             (CRFU, 255)]

data Ins = IAbout | ITime | IFullStatus | IOpenVavles | ICloseValves
         | ISuspend | IEnableProbe | IDisableProbe | IEmuWater | IEmuError
         | ISettingsLoad | ISettingsSave | IGetWiFi | IGetServ | IGetNTP
         | ISetWiFi | ISetServ | ISetWiFiPwd | ISetNTP
         | IReboot | IEcho | IUnsupported | IRFU
         deriving (Eq)

instance Enum Ins where
    fromEnum = fromJust . flip lookup ins_table
    toEnum = fromJust . flip lookup (map swap ins_table)
ins_table = [(IAbout, 0),
             (ITime, 1),
             (IFullStatus, 2),
             (IOpenVavles, 3),
             (ICloseValves, 4),
             (ISuspend, 5),
             (IEnableProbe, 6),
             (IDisableProbe, 7),
             (IEmuWater, 8),
             (IEmuError, 9),
             (ISettingsLoad, 10),
             (ISettingsSave, 11),
             (IGetWiFi, 100),
             (IGetServ, 101),
             (IGetNTP, 102),
             (ISetWiFi, 200),
             (ISetServ, 201),
             (ISetWiFiPwd, 202),
             (ISetNTP, 203),
             (IReboot, 252),
             (IEcho, 253),
             (IUnsupported, 254),
             (IRFU, 255)]

type Seq = Word8

type Len = Word8

type Dat = [Word8]

data Packet = Packet Cls Ins Seq Len Dat

