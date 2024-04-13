MONGO_EXPORT int mongo_env_sock_init( void ) {

    WSADATA wsaData;
    WORD wVers;
    static int called_once;
    static int retval;

    if (called_once) return retval;

    called_once = 1;
    wVers = MAKEWORD(1, 1);
    retval = (WSAStartup(wVers, &wsaData) == 0);

    return retval;
}