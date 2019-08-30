-- device's protocol commands, states, flows, etc.
module Proto where

import Data.Maybe (fromJust)
import Data.Tuple (swap)
import Data.Word (Word8)



from x = fromJust . ((flip lookup) x)

data Cls = CEcho | CInfo | CRequest | CResponse | CRFU
         deriving (Eq)

instance Enum Cls where
    fromEnum = from cls_table
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
    fromEnum = from ins_table
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

