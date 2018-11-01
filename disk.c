/* Available disk space block
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
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

#include "colors.h"
#include "icons.h"
#include "utils.h"

#define DISK_LOW 10
#define DISK_CRITICAL 5

typedef unsigned long long fssize_t;

int get_fssize(const char *path, fssize_t *avail, fssize_t *size) {
    struct statvfs buf;
    if (statvfs(path, &buf)) {
        perror(path);
        return -1;
    }
    *avail = (fssize_t)buf.f_bsize*buf.f_bavail;
    *size = (fssize_t)buf.f_frsize*buf.f_blocks;
    return 0;
}

int main(int argc, char *argv[]) {
    const char *path = getenv("BLOCK_INSTANCE");
    if (path == NULL || !strlen(path)) path = "/";

    fssize_t avail, size;
    if (get_fssize(path, &avail, &size)) {
        printf("%s\n\n\n", disk_icon);
        return 33;
    }

    int low = getenvi("_low", DISK_LOW);
    int critical = getenvi("_critical", DISK_CRITICAL);
    fssize_t prop = 100*avail/size;
    printf("%s%.2fG\n", disk_icon, (double)avail/(1L << 30));
    printf("%.2fG\n", (double)avail/(1L << 30));
    printf("%s\n", (prop < critical) ? base08 : (prop < low) ? base0A : "");
    return 0;
}
