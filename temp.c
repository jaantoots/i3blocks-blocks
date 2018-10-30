/* Temperature block
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
#include <dirent.h>

#include "colors.h"
#include "icons.h"

#define HWMON "/sys/devices/platform/coretemp.0/hwmon"
#define MAXLEN 512
#define TEMP_HIGH 70
#define TEMP_CRITICAL 85
#define TEMP_URGENT 95

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int getenvd(const char *name, const int val) {
    /* return environment variable or default */
    const char *env = getenv(name);
    return (env != NULL) ? atoi(env) : val;
}

int main(int argc, char *argv[]) {
    /* find directory in HWMON as these end with a non-standard integer */
    DIR *dir = opendir(HWMON);
    struct dirent *dp;
    char hwmon[MAXLEN];
    while ((dp = readdir(dir)) != NULL) {
        if (dp->d_type == DT_DIR && !(strncmp(dp->d_name, "hwmon", 5))) {
            snprintf(hwmon, sizeof hwmon, HWMON "/%s/temp1_input", dp->d_name);
            break;
        }
    }
    if (!(strlen(hwmon))) exit(EXIT_FAILURE);
    closedir(dir);

    /* read temperature */
    FILE *ftemp = fopen(hwmon, "r");
    if (ftemp == NULL) exit(EXIT_FAILURE);
    int temp;
    fscanf(ftemp, "%d", &temp);
    fclose(ftemp);
    temp /= 1000;

    /* print and format temperature */
    int high = getenvd("_high", TEMP_HIGH);
    int critical = getenvd("_critical", TEMP_CRITICAL);
    int urgent = getenvd("_urgent", TEMP_URGENT);
    printf("%s%d°C\n", temp_icons[MAX(0, MIN(4, (temp - 40)/10))], temp);
    printf("%d°C\n", temp);
    printf("%s\n",
            (temp >= critical) ? base08 :
                ((temp >= high) ? base0A : ""));
    if (temp >= urgent) return 33;
    return 0;
}
