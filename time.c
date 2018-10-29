/* Date and time display block
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
#include <time.h>

#include "colors.h"
#include "icons.h"

#define MAXLEN 256

int main(int argc, char *argv[]) {
    time_t now = time(NULL);
    char str[MAXLEN];
    if (strftime(str, sizeof str, "%F %T", localtime(&now))) {
        printf("%s%s\n", time_icon, str);
        printf("%s\n", str);
        printf("\n");
        return 0;
    }
    return 1;
}
