static void cleanChildren()
{
#ifdef HAVE_WAITPID
    int stat_loc;
#elif HAVE_WAIT3
    struct rusage rusage;
#if defined(__NeXT__)
    /* some systems need a union wait as argument to wait3 */
    union wait status;
#else
    int        status;
#endif
#endif

#if defined(HAVE_WAITPID) || defined(HAVE_WAIT3)
    int child = 1;
    int options = WNOHANG;
    while (child > 0)
    {
#ifdef HAVE_WAITPID
        child = (int)(waitpid(-1, &stat_loc, options));
#elif defined(HAVE_WAIT3)
        child = wait3(&status, options, &rusage);
#endif
        if (child < 0)
        {
            if (errno != ECHILD)
            {
                char buf[256];
                OFLOG_ERROR(dcmprscpLogger, "wait for child failed: " << OFStandard::strerror(errno, buf, sizeof(buf)));
            }
        }
    }
#endif
}