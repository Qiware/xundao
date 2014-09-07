#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#include "log.h"

int main(int argc, char *argv[])
{
    int idx, level;
    log_cycle_t *log;
    char path[FILE_PATH_MAX_LEN];

    level = log_get_level("error");

    snprintf(path, sizeof(path), "../log/%s.log", basename(argv[0]));

    log = log_init(level, path);
    if (NULL == log)
    {
        fprintf(stderr, "Init log failed!");
        return -1;
    }

    for (idx=0; idx<1000000; ++idx)
    {
        log_fatal(log, "This is just a test! [%d]", idx);
        log_error(log, "This is just a test! [%d]", idx);
        log_warn(log, "This is just a test! [%d]", idx);
        log_info(log, "This is just a test! [%d]", idx);
        log_debug(log, "This is just a test! [%d]", idx);
        log_info(log, "This is just a test! [%d]", idx);
    }

    log_destroy(&log);

    return 0;
}
