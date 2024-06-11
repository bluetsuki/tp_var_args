#include "ulibc.h"

static void help() {
    char msg[] = "\n\
PROG      : execute program PROG\n\
exit      : exit this shell\n\
help      : display this help\n\
sleep N   : sleep N milliseconds (preemptive)\n\
ticks     : show the current ticks value and timer frequency\n";
    puts(msg);
}

static void run() {
    puts("Welcome to YoctOS Shell. Type \"help\" for a list of commands.\n");

    char buf[512];

    while (1) {
        puts(">");
        read_string(buf);
        char *line = tolower(trim(buf));  // removes heading and trailing spaces and convert to lower case
        if (line[0] == 0) {
            putc('\n');
            continue;
        }
        else if (strcmp("help", line) == 0) {
            help();
        }
        else if (starts_with("sleep ", line)) {
            uint_t ms = atoi(trim(line + strlen("sleep ")));
            putc('\n');
            printf("Sleeping for %dms...\n", ms);
            sleep(ms);
        }
        else if (strcmp("ticks", line) == 0) {
            putc('\n');
            uint_t freq, ticks;
            timer_info(&freq, &ticks);
            printf("ticks=%d freq=%d\n", ticks, freq);
        }
        else if (strcmp("exit", line) == 0) {
            puts("\nBye.\n");
            exit();
        }
        // Attempts to run the specified file
        else {
            putc('\n');
            int id = task_exec(line, 0, NULL);
            if (id == -1) {
                printf("Failed executing \"%s\"\n", line);
            }
        }
    }
}

void main() {
    run();
}
