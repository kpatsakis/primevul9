OsVendorInit(void)
{
    static Bool beenHere = FALSE;

    signal(SIGCHLD, SIG_DFL);   /* Need to wait for child processes */

    if (!beenHere) {
        umask(022);
        xf86LogInit();
    }

    /* Set stderr to non-blocking. */
#ifndef O_NONBLOCK
#if defined(FNDELAY)
#define O_NONBLOCK FNDELAY
#elif defined(O_NDELAY)
#define O_NONBLOCK O_NDELAY
#endif

#ifdef O_NONBLOCK
    if (!beenHere) {
        if (xf86PrivsElevated()) {
            int status;

            status = fcntl(fileno(stderr), F_GETFL, 0);
            if (status != -1) {
                fcntl(fileno(stderr), F_SETFL, status | O_NONBLOCK);
            }
        }
    }
#endif
#endif

    beenHere = TRUE;
}