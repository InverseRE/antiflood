/* -*-*- mode: c++ -*-*- */

/*
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
*/

#include <Arduino.h>

#include "debug.h"
#include "proto.h"
#include "storage.h"
#include "ntp_time.h"

/** Packet class. */
enum Class : byte {
    cEcho         = 0,                      /**< a packet should be echoed as cInfo */
    cInfo         = 1,                      /**< a general information, no response required */
    cRequest      = 2,                      /**< a request */
    cResponse     = 3,                      /**< a response */
    cRFU          = 255                     /**< reserved */
};

/** Container/data type. */
enum Instruction : byte {
    iAbout        = 0,                      /**< info about firmware */
    iTime         = 1,                      /**< report/set local time */
    iFullStatus   = 2,                      /**< detectors & valves status */
    iOpenValves   = 3,                      /**< open all valves command */
    iCloseValves  = 4,                      /**< close all valves command */
    iSuspend      = 5,                      /**< enter power save mode */
    iEnableProbe  = 6,                      /**< enable probes */
    iDisableProbe = 7,                      /**< disable probes */
    iGetSetting   = 8,                      /**< read settings */
    iSetSetting   = 9,                      /**< append/update settings */
    iEmuWater     = 10,                     /**< emulate WATER on probes */
    iEmuError     = 11,                     /**< emulate ERROR on probes */
    iEcho         = 253,                    /**< echoed packet */
    iUnsupported  = 254,                    /**< unsupported request */
    iRFU          = 255                     /**< reserved */
};

class __attribute__((packed)) Packet {
private:
    Class _cla;                             /**< class of the packet */
    Instruction _ins;                       /**< included data type */
    byte _seqnum;                           /**< request/response, both have the same numbers */
    byte _data_size;                        /**< raw data length */
    byte _data[56];                         /**< raw data, structured acording to it's type */

    byte hdr_size(void) const {
        return 4;
    }

public:
    Packet()
        : _cla(cInfo), _ins(iUnsupported), _seqnum(0), _data_size(0) {
    }

    Packet(Class cla, Instruction ins, byte seqnum, byte data_size, const byte* data)
        : _cla(cla), _ins(ins), _seqnum(seqnum), _data_size(data_size) {
        if (_data_size > sizeof(_data)) {
            memset(_data, 0, sizeof(_data));
        } else {
            memcpy(_data, data, _data_size);
        }
    }

    bool validate(byte size) const {
        return _data_size <= sizeof(_data) && _data_size <= size - hdr_size();
    }

    byte raw(byte* buf, byte buf_size) const {
        byte packet_size = hdr_size() + _data_size;
        if (buf_size < packet_size) {
            return 0;
        }
        memcpy(buf, this, packet_size);
        return packet_size;
    }

    Class cla(void) const {
        return _cla;
    }

    Instruction ins(void) const {
        return _ins;
    }

    void trx_info_back(void) {
        _cla = cInfo;
        _ins = iEcho;
        _data_size = 0;
    }

    void trx_about(const String& msg) {
        byte len = msg.length();
        _data_size = len < sizeof(_data) ? len : sizeof(_data);
        _cla = cResponse;
        _ins = iAbout;
        memcpy(_data, msg.c_str(), _data_size);
    }

    unsigned long trx_time(String (*get_ntp_server)(void), const NetServer& ns, byte* buf, byte size) {
        byte act_len = ntp_make_request(buf, size);
        unsigned long utc = 0;
        char ntp_pool[64] = {0};
        String ntp = get_ntp_server();
        ntp.toCharArray(ntp_pool, sizeof(ntp_pool));
        SWS.print(ntp);
        ns.write(ntp_pool, 123, buf, act_len);
        ns.tx();
        delay(1000);
        if (ns.available()) {
            DPC("Responce!") ;
            act_len = ns.read(buf, size);
            DPV("ACT LEN : ", act_len);
            utc = ntp_parse_responce(buf, act_len);
        }
        _data_size = sizeof(utc);
        _cla = cResponse;
        _ins = iTime;
        memcpy(_data, &utc, _data_size);
    }

    void trx_full_status(byte (*state)(byte* buf, byte buf_max_size)) {
        _data_size = state(_data, sizeof(_data));
        _cla = cResponse;
        _ins = 1 == _data_size ? iUnsupported : iFullStatus;
    }

    void trx_get_setting(int (*get_setting)(byte type, byte* buf, byte* size)) {
        byte size = sizeof(_data) - sizeof(int);
        int res = get_setting(_data[0], _data, &size);
        _cla = cResponse;
        _ins = iGetSetting;
        if (res == ST_OK || res == ST_NOT_ENAUGHT) {
            _data_size = size;
            memcpy(_data + size, &res, sizeof(res));
        } else {
            _data_size = sizeof(res);
            memcpy(_data, &res, sizeof(res));
        }
    }

    void trx_set_setting(int (*set_setting)(byte type, const byte* data, byte size)) {
        int res = set_setting(_data[0], _data + 2, _data[1]);
        _cla = cResponse;
        _ins = iSetSetting;
        _data_size = sizeof(res);
        memcpy(_data, &res, sizeof(res));
    }


    void trx_open(bool (*open)(void)) {
        bool res = open();
        _data_size = sizeof(res);
        _cla = cResponse;
        _ins = iOpenValves;
        memcpy(_data, &res, _data_size);
    }

    void trx_close(bool (*close)(void)) {
        bool res = close();
        _data_size = sizeof(res);
        _cla = cResponse;
        _ins = iCloseValves;
        memcpy(_data, &res, _data_size);
    }

    void trx_suspend(bool (*suspend)(void)) {
        bool res = suspend();
        _data_size = sizeof(res);
        _cla = cResponse;
        _ins = iSuspend;
        memcpy(_data, &res, _data_size);
    }

    void trx_enable_probe(bool (*enable)(byte idx)) {
        bool res = _data_size == 1 && enable(_data[0]);
        _data_size = sizeof(res);
        _cla = cResponse;
        _ins = iEnableProbe;
        memcpy(_data, &res, _data_size);
    }

    void trx_disable_probe(bool (*disable)(byte idx, unsigned long duration)) {
        bool res = _data_size == 5 && disable(_data[0],
                  ((unsigned long)_data[1] << 24) | ((unsigned long)_data[2]) << 16
                | ((unsigned long)_data[3] << 8) | ((unsigned long)_data[4]));
        _data_size = sizeof(res);
        _cla = cResponse;
        _ins = iDisableProbe;
        memcpy(_data, &res, _data_size);
    }

    void trx_emu_water(bool (*emu_water)(byte idx, bool immediately)) {
        bool res = _data_size == 2 && emu_water(_data[0], _data[1]);
        _data_size = sizeof(res);
        _cla = cResponse;
        _ins = iEmuWater;
        memcpy(_data, &res, _data_size);
    }

    void trx_emu_error(bool (*emu_error)(byte idx, bool immediately)) {
        bool res = _data_size == 2 && emu_error(_data[0], _data[1]);
        _data_size = sizeof(res);
        _cla = cResponse;
        _ins = iEmuError;
        memcpy(_data, &res, _data_size);
    }

    void trx_unsupported(void) {
        memcpy(_data, this, hdr_size());
        _data_size = hdr_size();
        _cla = cResponse;
        _ins = iUnsupported;
    }
};

ProtoSession::ProtoSession(const Ticker& ticker, NetServer& server)
        : _ticker(ticker), _server(server)
{
}

void ProtoSession::setup(void)
{
    dPC("proto: setup");
}

ProtoAction ProtoSession::action(
        byte (*state)(byte* buf, byte buf_max_size),
        bool (*open)(void),
        bool (*close)(void),
        bool (*suspend)(void),
        bool (*enable)(byte idx),
        bool (*disable)(byte idx, unsigned long duration),
        int (*get_setting)(byte type, byte* buff, byte* size),
        int (*set_setting)(byte type, const byte* data, byte size),
        bool (*emu_water)(byte idx, bool immediately),
        bool (*emu_error)(byte idx, bool immediately),
        String (*get_ntp_server)(void))
{
    byte reads_limit = 2; // amount of packets parsed at a time
    ProtoAction action = PROTO_UNKNOWN;
    byte buf[sizeof(Packet)];
    unsigned len = 0;

    /* till data is incoming and enough buffer left */
    while (_server.rx() && _server.available() && len < sizeof(buf) && reads_limit--) {

        /* get some data */
        len += _server.read(buf + len, sizeof(buf) - len);

        dPA("proto: in", buf, len);

        /* parse packet */
        Packet* pkt = (Packet*)buf;
        if (!pkt->validate(len)) {
            /* get more data if uncompleted */
            dPC("proto: rx more");
            continue;
        }

        /* switch by class */
        switch (pkt->cla()) {
        case cRequest:
            dPC("proto: cRequest");
            break;
        case cResponse:
            dPC("proto: cResponse");
            break;
        case cEcho:
            dPC("proto: cEcho");
            pkt->trx_info_back();
            len = pkt->raw(buf, sizeof(buf));
            _server.write(buf, len);
            _server.tx();
        case cInfo:
            dPC("proto: cInfo");
        case cRFU:
            iPC("proto: cRFU");
        default:
            iPC("proto: unknown cla");
            len = 0;
            continue;
        }

        /* switch by requested action */
        switch (pkt->ins())  {
        case iAbout:        pkt->trx_unsupported();            action = PROTO_UNKNOWN; break;
        case iTime:         pkt->trx_time(get_ntp_server, _server, buf, sizeof(buf));
                                                               action = PROTO_TIME;    break;
        case iFullStatus:   pkt->trx_full_status(state);       action = PROTO_STATE;   break;
        case iOpenValves:   pkt->trx_open(open);               action = PROTO_OPEN;    break;
        case iCloseValves:  pkt->trx_close(close);             action = PROTO_CLOSE;   break;
        case iSuspend:      pkt->trx_suspend(suspend);         action = PROTO_SUSPEND; break;
        case iEnableProbe:  pkt->trx_enable_probe(enable);     action = PROTO_EN_PROBE; break;
        case iDisableProbe: pkt->trx_disable_probe(disable);   action = PROTO_DIS_PROBE; break;
        case iGetSetting:   pkt->trx_get_setting(get_setting); action = PROTO_GET_SETTING; break;
        case iSetSetting:   pkt->trx_set_setting(set_setting); action = PROTO_SET_SETTING; break;
        case iEmuWater:     pkt->trx_emu_water(emu_water);     action = PROTO_EMU_WATER; break;
        case iEmuError:     pkt->trx_emu_error(emu_error);     action = PROTO_EMU_ERROR; break;
        case iEcho:         pkt->trx_unsupported();            action = PROTO_UNKNOWN; break;
        case iUnsupported:  pkt->trx_unsupported();            action = PROTO_UNKNOWN; break;
        case iRFU:          pkt->trx_unsupported();            action = PROTO_UNKNOWN; break;
        default:            pkt->trx_unsupported();            action = PROTO_UNKNOWN; break;
        }

        /* write out */
        len = pkt->raw(buf, sizeof(buf));
        _server.write(buf, len);
        dPA("proto: out", buf, len);

        /* send data back immediately */
        _server.tx(); // XXX: but this can be done later as a bunch of packets are ready

        /* packet has been processed, get another one */
        len = 0;
    }

    return action;
}
