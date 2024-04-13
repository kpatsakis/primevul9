int mongo_env_socket_connect( mongo *conn, const char *host, int port ) {
    struct sockaddr_in sa;
    socklen_t addressSize;
    int flag = 1;

    if ( ( conn->sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
        conn->sock = 0;
        conn->err = MONGO_CONN_NO_SOCKET;
        return MONGO_ERROR;
    }

    memset( sa.sin_zero , 0 , sizeof( sa.sin_zero ) );
    sa.sin_family = AF_INET;
    sa.sin_port = htons( port );
    sa.sin_addr.s_addr = inet_addr( host );
    addressSize = sizeof( sa );

    if ( connect( conn->sock, ( struct sockaddr * )&sa, addressSize ) == -1 ) {
        mongo_env_close_socket( conn->sock );
        conn->connected = 0;
        conn->sock = 0;
        conn->err = MONGO_CONN_FAIL;
        return MONGO_ERROR;
    }

    setsockopt( conn->sock, IPPROTO_TCP, TCP_NODELAY, ( char * ) &flag, sizeof( flag ) );

    if( conn->op_timeout_ms > 0 )
        mongo_env_set_socket_op_timeout( conn, conn->op_timeout_ms );

    conn->connected = 1;

    return MONGO_OK;
}