/* Wireless block
 * Copyright (C) 2018 Jaan Toots <jaan@jaantoots.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iwlib.h>

#include "colors.h"
#include "icons.h"

#define MAXLEN 512
#define NET "/sys/class/net"
#define TERMINAL "i3-sensible-terminal"

/* from wireless_tools.29/iwconfig.c */
static int get_info(int skfd, char * ifname, struct wireless_info * info) {
    struct iwreq wrq;
    memset((char *) info, 0, sizeof (struct wireless_info));
  
    /* get basic information */
    if(iw_get_basic_config(skfd, ifname, &(info->b)) < 0) {
        /* if no wireless name : no wireless extensions */
        /* but let's check if the interface exists at all */
        struct ifreq ifr;
        memcpy(ifr.ifr_name, ifname, IFNAMSIZ);
        if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) return(-ENODEV);
        else return(-ENOTSUP);
    }
  
    /* get bit rate */
    if(iw_get_ext(skfd, ifname, SIOCGIWRATE, &wrq) >= 0) {
        info->has_bitrate = 1;
        memcpy(&(info->bitrate), &(wrq.u.bitrate), sizeof (iwparam));
    }

    return(0);
}

int main(int argc, char *argv[]) {
    /* get device */
    char *ifname = getenv("BLOCK_INSTANCE");
    if (ifname == NULL || !strlen(ifname)) {
        /* find wireless devices */
        DIR *dir = opendir(NET);
        struct dirent *dp;
        while ((dp = readdir(dir)) != NULL) {
            if (!strncmp(dp->d_name, "wl", 2)) {
                char ifdir[MAXLEN];
                snprintf(ifdir, sizeof ifdir, NET "/%s/wireless", dp->d_name);
                struct stat buf;
                if (!stat(ifdir, &buf)) {
                    ifname = dp->d_name;
                    break;
                }
            }
        }
        closedir(dir);
        if (ifname == NULL || !strlen(ifname)) return 0;
    }

    /* create a channel to the NET kernel. */
    int skfd;
    if((skfd = iw_sockets_open()) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    wireless_info info;
    switch (get_info(skfd, ifname, &info)) {
        case 0:
            break;
        case -ENOTSUP:
            return 0;
        default:
            return 1;
    }

    printf("%s%s %.3gGHz %.3gMbps\n", wifi_icon, info.b.essid, info.b.freq/1e9, info.bitrate.value/1e6);

    /* close the socket. */
    iw_sockets_close(skfd);
    return 0;
}
