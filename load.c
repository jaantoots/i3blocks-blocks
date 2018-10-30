/* Load block
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
#include <unistd.h>
#include <sys/sysinfo.h>

#include "colors.h"
#include "icons.h"

#define TERMINAL "i3-sensible-terminal"

void htop() {
    /* fork htop */
    int pid = fork();
    if (!(pid)) {
        if (setpgid(0, 0)) exit(EXIT_FAILURE);
        pid = fork();
        if (pid < 0) exit(EXIT_FAILURE);
        if (pid > 0) exit(EXIT_SUCCESS);
        for (long i = sysconf(_SC_OPEN_MAX); i >= 0; --i) close(i);
        execlp(TERMINAL, TERMINAL, "-e", "htop", NULL);
    }
}

int main(int argc, char *argv[]) {
    /* handle mouse buttons */
    bool longf = false;
    const char *button = getenv("BLOCK_BUTTON");
    if (button != NULL) {
        switch (atoi(button)) {
            case 1: longf = true;
                    break;
            case 3: htop();
                    break;
        }
    }

    FILE *fload = fopen("/proc/loadavg", "r");
    if (fload == NULL) return 1;
    float load[3];
    fscanf(fload, "%f %f %f", load, load + 1, load + 2);
    fclose(fload);

    if (longf)
        printf("%s%.2f %.2f %.2f\n", load_icon, load[0], load[1], load[2]);
    else
        printf("%s%.2f\n", load_icon, load[0]);
    printf("%.2f\n", load[0]);
    printf("%s\n", (load[0] > get_nprocs()) ? base08 : "");
    return 0;
}
