/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2011 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef TPUART_H
#define TPUART_H
#include "iobuf.h"
#include "eibnetip.h"
#include "layer2.h"
#include "lpdu.h"

/** TPUART user mode driver */
class TPUART_Base:public Layer2
{
protected:
  /** device connection */
  int fd;
  /** queueing */
  SendBuf sendbuf;
  RecvBuf recvbuf;
  size_t read_cb(uint8_t *buf, size_t len);
  void error_cb();

  bool ackallgroup;
  bool ackallindividual;

  /** process a received frame */
  virtual void RecvLPDU (const uchar * data, int len);
  void process_read(bool timed_out);

  virtual const char *Name() = 0;
  virtual void send_reset();
  virtual void after_lpdu_received() {}
  void setup_buffers();

  /** main loop state */
  ev::async trigger; void trigger_cb(ev::async &w, int revents);
  ev::timer timer; void timer_cb(ev::timer &w, int revents);
  ev::timer sendtimer; void sendtimer_cb(ev::timer &w, int revents);
  ev::timer watchdogtimer; void watchdogtimer_cb(ev::timer &w, int revents);
  LPDU *l = nullptr;
  Queue <LPDU *> send_q;

  CArray in, sendheader;
  int watch = 0;
  int retry = 0;
  bool acked = false;
  bool recvecho = false;
  bool skip_char = false;

public:
  TPUART_Base (L2options *opt);
  ~TPUART_Base ();
  bool init (Layer3 *l3);

  void send_L_Data (L_Data_PDU * l);

  bool enterBusmonitor ();
  bool leaveBusmonitor ();

  bool Open ();
};

#endif
