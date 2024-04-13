static int mongo_env_unix_socket_connect( mongo *conn, const char *sock_path ) {
    struct sockaddr_un addr;
    int status, len;

    conn->connected = 0;

    conn->sock = socket( AF_UNIX, SOCK_STREAM, 0 );

    if ( conn->sock < 0 ) {
        conn->sock = 0;
        return MONGO_ERROR;
    }

    addr.sun_family = AF_UNIX;
    strncpy( addr.sun_path, sock_path, sizeof(addr.sun_path) - 1 );
    len = sizeof( addr );

    status = connect( conn->sock, (struct sockaddr *) &addr, len );
    if( status < 0 ) {
        mongo_env_close_socket( conn->sock );
        conn->sock = 0;
        conn->err = MONGO_CONN_FAIL;
        return MONGO_ERROR;
    }

    conn->connected = 1;

    return MONGO_OK;
}