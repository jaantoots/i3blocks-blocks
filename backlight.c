/* Backlight block
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
#include <unistd.h>

#include "colors.h"
#include "icons.h"
#include "utils.h"

#define MAXLEN 512
#define backlight "xbacklight"

int main(int argc, char *argv[]) {
    char * const args[] = { backlight, "-get", NULL };
    int fildes[2];
    int status = pipe(fildes);
    if (status) {
        perror("pipe");
        return 1;
    }
    char out[MAXLEN] = {'\0'};
    int pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            return 1;
        case 0: /* child */
            close(fildes[0]);
            dup2(fildes[1], 1);
            execvp(backlight, args);
            perror("exec");
            return 1;
        default: /* parent */
            close(fildes[1]);
            int i = 0;
            while (read(fildes[0], out + i, sizeof (char)) > 0
                    && i < sizeof out) ++i;
            close(fildes[0]);
    }
    float value;
    sscanf(out, "%f", &value);

    printf("%s%.0f%%\n", backlight_icon, value);
    printf("%.0f%%\n", value);
    printf("\n");
    return 0;
}
