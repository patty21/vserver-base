/*  
    VTun - Virtual Tunnel over TCP/IP network.

    Copyright (C) 1998-2008  Maxim Krasnyansky <max_mk@yahoo.com>

    VTun has been derived from VPPP package by Maxim Krasnyansky. 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 */

/*
 * $Id: tun_dev.c,v 1.4.2.1 2008/01/07 22:36:22 mtbishop Exp $
 */ 

#include "config.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>

#include "vtun.h"
#include "lib.h"

//this function is used on old systems. It renames the interface via its own 
//fd. NOT TESTED
//IN: name - prefix of interface (e.g. tap not tap0)
//OUT: name - interface with number that was successful
static void rename_if(char* name)
{
  struct ifreq r;
  int  ctl_fd, k;
  char newname[IFNAMSIZ + 1];
  char n[4];
           
          if(vtun.ifname && *vtun.ifname)
          {
            vtun_syslog(LOG_DEBUG,"VTUN: ifname=%s\n", vtun.ifname);	

            if ((ctl_fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
            {
              vtun_syslog(LOG_ERR, "VTUN: Cannot open control_fd\n"); 
            }
            else
            {  
	      /* try to rename interface until it has success */
	      for(k=0; k < 255; k++)
              {
                strncpy(newname, vtun.ifname, IFNAMSIZ);

                /* truncate to reserve space for number and '\0' */
                newname[IFNAMSIZ-3]='\0';
                sprintf(n,"%ld", k); 
                strcat(newname,n); 

                vtun_syslog(LOG_DEBUG,"VTUN: newname=%s\n", newname);	

                strncpy(r.ifr_name, name, IFNAMSIZ);
                strncpy(r.ifr_newname, newname, IFNAMSIZ);
                if (ioctl(ctl_fd, SIOCSIFNAME, &r) >= 0)
                  strncpy(name, newname, IFNAMSIZ); 
              }
              close(ctl_fd);
            }
          }
}

/* 
 * Allocate TUN device, returns opened fd. 
 * Stores dev name in the first arg(must be large enough).
 */  
static int tun_open_common0(char *dev, int istun)
{
    char tunname[14];
    int i, fd, err;

    if( *dev ) {
       sprintf(tunname, "/dev/%s", dev);
       vtun_syslog(LOG_INFO,"VTUN: used ifname=%s\n", dev);	
       return open(tunname, O_RDWR);
    }

    sprintf(tunname, "/dev/%s", istun ? "tun" : "tap");
    err = 0;
    for(i=0; i < 255; i++){
       sprintf(tunname + 8, "%d", i);
       /* Open device */
       if( (fd=open(tunname, O_RDWR)) > 0 ) {

          strcpy(dev, tunname + 5);
	  rename_if(dev);

          vtun_syslog(LOG_INFO,"VTUN: used ifname=%s\n", dev);	

          return fd;
       }
       else if (errno != ENOENT)
          err = errno;
       else if (i)	/* don't try all 256 devices */
          break;
    }
    if (err)
	errno = err;
    return -1;
}

#ifdef HAVE_LINUX_IF_TUN_H /* New driver support */
#include <linux/if_tun.h>

#ifndef OTUNSETNOCSUM
/* pre 2.4.6 compatibility */
#define OTUNSETNOCSUM  (('T'<< 8) | 200) 
#define OTUNSETDEBUG   (('T'<< 8) | 201) 
#define OTUNSETIFF     (('T'<< 8) | 202) 
#define OTUNSETPERSIST (('T'<< 8) | 203) 
#define OTUNSETOWNER   (('T'<< 8) | 204)
#endif

static int tun_open_common(char *dev, int istun)
{
    struct ifreq ifr;
    int fd;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
       return tun_open_common0(dev, istun);

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = (istun ? IFF_TUN : IFF_TAP) | IFF_NO_PI;
    if (*dev)
    {
       strncpy(ifr.ifr_name, dev, IFNAMSIZ);

       if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
          if (errno == EBADFD) {
	     /* Try old ioctl */
 	     if (ioctl(fd, OTUNSETIFF, (void *) &ifr) < 0) 
	        goto failed;
          } else
             goto failed;
       } 
    }
    else
    {
      char newname[IFNAMSIZ + 1];
      char n[4];
      int k;

      if(vtun.ifname && *vtun.ifname)
      {
	/* try to rename interface until it has success */
	for(k=0; k < 255; k++)
        {
          strncpy(newname, vtun.ifname, IFNAMSIZ);

          /* truncate to reserve space for number and '\0' */
          newname[IFNAMSIZ-3]='\0';
          sprintf(n,"%ld", k); 
          strcat(newname,n); 

          vtun_syslog(LOG_DEBUG,"VTUN: check ifname=%s\n", newname);	

          strncpy(ifr.ifr_name, newname, IFNAMSIZ);
          if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
             if (errno == EBADFD) { /* Try old ioctl */
 	        if (ioctl(fd, OTUNSETIFF, (void *) &ifr) >= 0) 
                   break;
             }
             continue;
          } 
          break;
        }//for
        if(k==255)
	  goto failed;
      }//if vtun.ifname 
      else
      { //get ifname with same function: returned in ifr.ifr_name
       if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
          if (errno == EBADFD) {
	     /* Try old ioctl */
 	     if (ioctl(fd, OTUNSETIFF, (void *) &ifr) < 0) 
	        goto failed;
          } else
             goto failed;
       } 
      }//else vtun.ifname
    } 

    strcpy(dev, ifr.ifr_name);
    vtun_syslog(LOG_INFO,"VTUN: use if name=%s\n", dev);	
    return fd;

failed:
    close(fd);
    return -1;
}

#else

# define tun_open_common(dev, type) tun_open_common0(dev, type)

#endif /* New driver support */

int tun_open(char *dev) { return tun_open_common(dev, 1); }
int tap_open(char *dev) { return tun_open_common(dev, 0); }

int tun_close(int fd, char *dev) { return close(fd); }
int tap_close(int fd, char *dev) { return close(fd); }

/* Read/write frames from TUN device */
int tun_write(int fd, char *buf, int len) { return write(fd, buf, len); }
int tap_write(int fd, char *buf, int len) { return write(fd, buf, len); }

int tun_read(int fd, char *buf, int len) { return read(fd, buf, len); }
int tap_read(int fd, char *buf, int len) { return read(fd, buf, len); }
