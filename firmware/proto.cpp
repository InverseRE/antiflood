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
    iEmuWater     = 8,                      /**< emulate WATER on probes */
    iEmuError     = 9,                      /**< emulate ERROR on probes */
    iGetWiFi      = 100,                    /**< read WiFi settings */
    iGetServ      = 101,                    /**< read Server settings */
    iGetNTP       = 102,                    /**< read NTP host setting */
    iSetWiFi      = 200,                    /**< write WiFi settings */
    iSetServ      = 201,                    /**< write Server settings */
    iSetWiFiPwd   = 202,                    /**< write WiFi settings (password) */
    iSetNTP       = 203,                    /**< write NTP host setting (name) */
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

    void trx_time(unsigned long (*time)(bool sync, bool ref, bool raw)) {
        unsigned long t = 3 == _data_size ? time(_data[0], _data[1], _data[2]) : 0;
        _data_size = sizeof(t);
        _cla = cResponse;
        _ins = iTime;
        memcpy(_data, &t, _data_size);
    }

    void trx_full_status(byte (*state)(byte* buf, byte buf_max_size)) {
        _data_size = state(_data, sizeof(_data));
        _cla = cResponse;
        _ins = 1 == _data_size ? iUnsupported : iFullStatus;
    }

    void trx_get_wifi(byte (*get_wifi)(byte* buf, byte buf_max_size)) {
        _data_size = get_wifi(_data, sizeof(_data));
        _cla = cResponse;
        _ins = iGetWiFi;
    }

    void trx_set_wifi(bool (*set_wifi)(const byte* buf, byte buf_size)) {
        bool res = set_wifi(_data, _data_size);
        _cla = cResponse;
        _ins = iSetWiFi;
        _data_size = sizeof(res);
        memcpy(_data, &res, sizeof(res));
    }

    void trx_set_wifi_pwd(bool (*set_wifi_pwd)(const byte* buf, byte buf_size)) {
        bool res = set_wifi_pwd(_data, _data_size);
        _cla = cResponse;
        _ins = iSetWiFiPwd;
        _data_size = sizeof(res);
        memcpy(_data, &res, sizeof(res));
    }

    void trx_get_serv(byte (*get_serv)(byte* buf, byte buf_max_size)) {
        _data_size = get_serv(_data, sizeof(_data));
        _cla = cResponse;
        _ins = iGetServ;
    }

    void trx_set_serv(bool (*set_serv)(const byte* buf, byte buf_size)) {
        bool res = set_serv(_data, _data_size);
        _cla = cResponse;
        _ins = iSetServ;
        _data_size = sizeof(res);
        memcpy(_data, &res, sizeof(res));
    }

    void trx_get_ntp(byte (*get_ntp)(byte* buf, byte buf_max_size)) {
        _data_size = get_ntp(_data, sizeof(_data));
        _cla = cResponse;
        _ins = iGetNTP;
    }

    void trx_set_ntp(bool (*set_ntp)(const byte* buf, byte buf_size)) {
        bool res = set_ntp(_data, _data_size);
        _cla = cResponse;
        _ins = iSetNTP;
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

void ProtoSession::action(
        unsigned long (*time)(bool sync, bool ref, bool raw),
        byte (*state)(byte* buf, byte buf_max_size),
        bool (*open)(void),
        bool (*close)(void),
        bool (*suspend)(void),
        bool (*enable)(byte idx),
        bool (*disable)(byte idx, unsigned long duration),
        bool (*emu_water)(byte idx, bool immediately),
        bool (*emu_error)(byte idx, bool immediately),
        byte (*get_wifi)(byte* buf, byte buf_max_size),
        bool (*set_wifi)(const byte* buf, byte buf_size),
        bool (*set_wifi_pwd)(const byte* buf, byte buf_size),
        byte (*get_serv)(byte* buf, byte buf_max_size),
        bool (*set_serv)(const byte* buf, byte buf_size),
        byte (*get_ntp)(byte* buf, byte buf_max_size),
        bool (*set_ntp)(const byte* buf, byte buf_size))
{
    byte reads_limit = 2; // amount of packets parsed at a time
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
        case iAbout:        pkt->trx_unsupported();              break;
        case iTime:         pkt->trx_time(time);                 break;
        case iFullStatus:   pkt->trx_full_status(state);         break;
        case iOpenValves:   pkt->trx_open(open);                 break;
        case iCloseValves:  pkt->trx_close(close);               break;
        case iSuspend:      pkt->trx_suspend(suspend);           break;
        case iEnableProbe:  pkt->trx_enable_probe(enable);       break;
        case iDisableProbe: pkt->trx_disable_probe(disable);     break;
        case iEmuWater:     pkt->trx_emu_water(emu_water);       break;
        case iEmuError:     pkt->trx_emu_error(emu_error);       break;
        case iGetWiFi:      pkt->trx_get_wifi(get_wifi);         break;
        case iGetServ:      pkt->trx_get_serv(get_serv);         break;
        case iGetNTP:       pkt->trx_get_ntp(get_ntp);           break;
        case iSetWiFi:      pkt->trx_set_wifi(set_wifi);         break;
        case iSetServ:      pkt->trx_set_serv(set_serv);         break;
        case iSetWiFiPwd:   pkt->trx_set_wifi_pwd(set_wifi_pwd); break;
        case iSetNTP:       pkt->trx_set_ntp(set_ntp);           break;
        case iEcho:         pkt->trx_unsupported();              break;
        case iUnsupported:  pkt->trx_unsupported();              break;
        case iRFU:          pkt->trx_unsupported();              break;
        default:            pkt->trx_unsupported();              break;
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
}
