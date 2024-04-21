#ifndef TUN_INTERFACE_H
#define TUN_INTERFACE_H

#include "common.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>

int createTunInterface();

#endif