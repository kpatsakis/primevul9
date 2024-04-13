MONGO_EXPORT int mongo_env_sock_init( void ) {

#if defined(_WIN32)
    WSADATA wsaData;
    WORD wVers;
#elif defined(SIGPIPE)
    struct sigaction act;
#endif

    static int called_once;
    static int retval;
    if (called_once) return retval;
    called_once = 1;

#if defined(_WIN32)
    wVers = MAKEWORD(1, 1);
    retval = (WSAStartup(wVers, &wsaData) == 0);
#elif defined(MACINTOSH)
    GUSISetup(GUSIwithInternetSockets);
    retval = 1;
#elif defined(SIGPIPE)
    retval = 1;
    if (sigaction(SIGPIPE, (struct sigaction *)NULL, &act) < 0)
        retval = 0;
    else if (act.sa_handler == SIG_DFL) {
        act.sa_handler = SIG_IGN;
        if (sigaction(SIGPIPE, &act, (struct sigaction *)NULL) < 0)
            retval = 0;
    }
#endif
    return retval;
}