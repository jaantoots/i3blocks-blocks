/* Battery block
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

#include "colors.h"
#include "icons.h"
#include "utils.h"

#define BAT "/sys/class/power_supply/"
#define BAT0 "BAT0"
#define MAXLEN 512
#define BAT_LOW 25
#define BAT_CRITICAL 15
#define BAT_URGENT 10

#define BATREAD(dir, fmt, var) batread(dir, #var, fmt, &var);

void batread(const char *dir, const char *fname, const char *fmt, void *value) {
    /* read value from specified file */
    char name[MAXLEN] = {'\0'};
    strncpy(name, dir, MAXLEN - 1);
    strncat(name, fname, MAXLEN - strlen(name) - 1);
    FILE *file = fopen(name, "r");
    if (file == NULL) {
        perror(name);
        return;
    }
    fscanf(file, fmt, value);
    if (ferror(file)) perror(name);
    fclose(file);
}

int main(int argc, char *argv[]) {
    /* get battery path */
    const char *instance = getenv("BLOCK_INSTANCE");
    char dir[MAXLEN];
    snprintf(dir, MAXLEN, BAT "/%s/",
            (instance != NULL && strlen(instance) > 0) ? instance : BAT0);

    /* read battery status values */
    char *status;
    long long capacity, charge_full, charge_now, current_now, voltage_now;
    BATREAD(dir, "%ms", status);
    BATREAD(dir, "%Ld", capacity);
    BATREAD(dir, "%Ld", charge_full);
    BATREAD(dir, "%Ld", charge_now);
    BATREAD(dir, "%Ld", current_now);
    BATREAD(dir, "%Ld", voltage_now);

    /* format block */
    const char *icon = power_icons[0];
    const char *color = NULL;
    long long time = 0;
    int low = getenvi("_low", BAT_LOW);
    int critical = getenvi("_critical", BAT_CRITICAL);
    int urgent = getenvi("_urgent", BAT_URGENT);
    if (!(strcmp(status, "Charging"))) {
        icon = power_icons[1];
        time = 3600*(charge_full - charge_now)/current_now;
    }
    else if (!(strcmp(status, "Full"))) {
        icon = power_icons[2];
        color = base0B;
    }
    else if (!(strcmp(status, "Discharging"))) {
        icon = battery_icons[(capacity + 12)/25];
        time = 3600*charge_now/current_now;
        if (capacity <= critical) color = base08;
        else if (capacity <= low) color = base0A;
    }
    free(status);

    /* print block */
    double percent = (double)100*charge_now/charge_full;
    double power = (double)voltage_now*current_now/1e12;
    if (time) printf("%s%.2lf%% %02lld:%02lld %.1fW\n", icon, percent,
            time/3600, (time % 3600)/60, power);
    else printf("%s%.2lf%%\n", icon, percent);
    printf("%.2lf%%\n", percent);
    printf("%s\n", (color != NULL) ? color : "");
    if (capacity <= urgent) return 33;
    return 0;
}
