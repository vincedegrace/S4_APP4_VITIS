/**
 * @file accel.h
 * @brief accel protocol
 *
 * @section License
 *
 * Copyright (C) 2010-2018 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.9.0
 **/

#ifndef _ACCEL_H
#define _ACCEL_H

//Dependencies
#include <packetTask.h>
#include "cyclone_tcp/core/net.h"
#include "cyclone_tcp/core/socket.h"

//Stack size required to run the accel service
#ifndef ACCEL_SERVICE_STACK_SIZE
   #define ACCEL_SERVICE_STACK_SIZE 600
#elif (ACCEL_SERVICE_STACK_SIZE < 1)
   #error ACCEL_SERVICE_STACK_SIZE parameter is not valid
#endif

//Priority at which the accel service should run
#ifndef ACCEL_SERVICE_PRIORITY
   #define ACCEL_SERVICE_PRIORITY OS_TASK_PRIORITY_NORMAL
#endif

//Size of the buffer for input/output operations
#ifndef ACCEL_BUFFER_SIZE
   #define ACCEL_BUFFER_SIZE 1537
#elif (ACCEL_BUFFER_SIZE < 1)
   #error ACCEL_BUFFER_SIZE parameter is not valid
#endif

//Maximum time the TCP accel server will wait before closing the connection
#ifndef ACCEL_TIMEOUT
   #define ACCEL_TIMEOUT 20000
#elif (ACCEL_TIMEOUT < 1)
   #error ACCEL_TIMEOUT parameter is not valid
#endif

//accel service port
#define ACCEL_PORT 8080

//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif


/**
 * @brief accel service context
 **/

typedef struct
{
   Socket *socket;
   char_t buffer[ACCEL_BUFFER_SIZE];
} AccelServiceContext;

extern AccelServiceContext context;



//UDP accel service related functions
error_t udpAccelStart(void);
void udpAccelTask(int newPacket, int send);

//C++ guard
#ifdef __cplusplus
   }
#endif

#endif
