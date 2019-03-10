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

#include <Arduino.h>
#include "proto.h"
#include "debug.h"

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
    iEcho         = 253,                    /**< echoed packet */
    iUnsupported  = 254,                    /**< unsupported request */
    iRFU          = 255                     /**< reserved */
};

#pragma push(pack, 0)

class Packet {
private:
    Class _cla;                             /**< class of the packet */
    Instruction _ins;                       /**< included data type */
    byte _seqnum;                           /**< request/response, both have the same numbers */
    byte _data_size;                        /**< raw data length */
    byte _xor;                              /**< magic 0 = cla^ins^seqnum^data_size^xor */
    byte _data[56];                         /**< raw data, structured acording to it's type */

    byte hdr_size(void) const {
        return 5;
    }

    void csum(void) {
        _xor = _cla ^ _ins ^ _seqnum ^ _data_size;
    }

public:
    Packet()
        : _cla(cInfo), _ins(iUnsupported), _seqnum(0), _data_size(0) {
        csum();
    }

    Packet(Class cla, Instruction ins, byte seqnum, byte data_size, const byte* data)
        : _cla(cla), _ins(ins), _seqnum(seqnum), _data_size(data_size) {
        csum();
        if (_data_size > sizeof(_data)) {
            _xor ^= 1;
            memset(_data, 0, sizeof(_data));
        } else {
            memcpy(_data, data, _data_size);
        }
    }

    bool validate(byte size) const {
        return _xor == _cla ^ _ins ^ _seqnum ^ _data_size
                && _data_size <= sizeof(_data)
                && _data_size <= size - hdr_size();
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
        csum();
    }

    void trx_about(const String& msg) {
        byte len = msg.length();
        _data_size = len < sizeof(_data) ? len : sizeof(_data);
        _cla = cResponse;
        _ins = iAbout;
        memcpy(_data, msg.c_str(), _data_size);
        csum();
    }

    void trx_time(unsigned long time) {
        _data_size = sizeof(time);
        _cla = cResponse;
        _ins = iTime;
        memcpy(_data, &time, _data_size);
        csum();
    }

    void trx_full_status(AppState app_state,
            const Led* leds, byte leds_cnt,
            const Probe* probes, byte probes_cnt,
            const Valve* valves, byte valves_cnt) {
        csum();
    }

    void trx_open(bool success) {
        _data_size = sizeof(success);
        _cla = cResponse;
        _ins = iOpenValves;
        memcpy(_data, &success, _data_size);
        csum();
    }

    void trx_close(bool success) {
        _data_size = sizeof(success);
        _cla = cResponse;
        _ins = iCloseValves;
        memcpy(_data, &success, _data_size);
        csum();
    }

    void trx_suspend(bool success) {
        _data_size = sizeof(success);
        _cla = cResponse;
        _ins = iSuspend;
        memcpy(_data, &success, _data_size);
        csum();
    }

    void trx_enable_probe(bool success) {
        _data_size = sizeof(success);
        _cla = cResponse;
        _ins = iEnableProbe;
        memcpy(_data, &success, _data_size);
        csum();
    }

    void trx_disable_probe(bool success) {
        _data_size = sizeof(success);
        _cla = cResponse;
        _ins = iDisableProbe;
        memcpy(_data, &success, _data_size);
        csum();
    }

    void trx_unsupported(void) {
        memcpy(_data, this, hdr_size());
        _data_size = hdr_size();
        _cla = cResponse;
        _ins = iUnsupported;
        csum();
    }
};

#pragma pop

ProtoSession::ProtoSession(const Ticker& ticker, const NetServer& server)
        : _ticker(ticker), _server(server)
{
}

void ProtoSession::setup(void)
{
}

ProtoAction ProtoSession::inform(
        AppState app_state,
        const Led* leds, byte leds_cnt,
        const Probe* probes, byte probes_cnt,
        const Valve* valves, byte valves_cnt)
{
    byte packets_limit = 1; // amount of packets parsed at a time
    ProtoAction action = PROTO_UNKNOWN;
    byte buf[255];
    int len = 0;

    /* till data is incoming and enough buffer left */
    while (_server.available() && len < sizeof(buf) && packets_limit) {

        /* get some data */
        len += _server.read(buf + len, sizeof(buf) - len);

        DPA("rxd", buf, len);

        /* parse packet */
        Packet* pkt = (Packet*)buf;
        if (!pkt->validate(len)) {
            /* get more data if uncompleted */
            DPC("get more data");
            continue;
        }

        /* switch by class */
        switch (pkt->cla()) {
        case cRequest:
            DPC("cRequest");
            break;
        case cResponse:
            DPC("cResponse");
            break;
        case cEcho:
            DPC("cEcho");
            pkt->trx_info_back();
            len = pkt->raw(buf, sizeof(buf));
            _server.write(buf, len);
            _server.tx();
        case cInfo:
            DPC("cInfo");
        case cRFU:
            DPC("cRFU");
        default:
            packets_limit -= 1;
            len = 0;
            continue;
        }

        /* switch by requested action */
        switch (pkt->ins())  {
        case iAbout:        pkt->trx_unsupported();        action = PROTO_UNKNOWN; break;
        case iTime:         pkt->trx_unsupported();        action = PROTO_UNKNOWN; break;
        case iFullStatus:   pkt->trx_full_status(
                app_state,
                leds, leds_cnt,
                probes, probes_cnt,
                valves, valves_cnt);                       action = PROTO_STATE;   break;
        case iOpenValves:   pkt->trx_open(false);          action = PROTO_OPEN;    break;
        case iCloseValves:  pkt->trx_close(false);         action = PROTO_CLOSE;   break;
        case iSuspend:      pkt->trx_suspend(false);       action = PROTO_SUSPEND; break;
        case iEnableProbe:  pkt->trx_enable_probe(false);  action = PROTO_UNKNOWN; break;
        case iDisableProbe: pkt->trx_disable_probe(false); action = PROTO_UNKNOWN; break;
        case iEcho:         pkt->trx_unsupported();        action = PROTO_UNKNOWN; break;
        case iUnsupported:  pkt->trx_unsupported();        action = PROTO_UNKNOWN; break;
        case iRFU:          pkt->trx_unsupported();        action = PROTO_UNKNOWN; break;
        default:            pkt->trx_unsupported();        action = PROTO_UNKNOWN; break;
        }

        /* write out */
        len = pkt->raw(buf, sizeof(buf));
        _server.write(buf, len);

        /* send data back immediately */
        _server.tx(); // XXX: but this can be done later as a bunch of packets are ready

        /* packet has been processed, get another one */
        packets_limit -= 1;
        len = 0;
    }

    return action;
}
