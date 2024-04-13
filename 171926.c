int mongo_env_socket_connect( mongo *conn, const char *host, int port ) {
    char port_str[NI_MAXSERV];
    char errstr[MONGO_ERR_LEN];
    int status;

    struct addrinfo ai_hints;
    struct addrinfo *ai_list = NULL;
    struct addrinfo *ai_ptr = NULL;

    conn->sock = 0;
    conn->connected = 0;

    bson_sprintf( port_str, "%d", port );

    memset( &ai_hints, 0, sizeof( ai_hints ) );
    ai_hints.ai_family = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;

    status = getaddrinfo( host, port_str, &ai_hints, &ai_list );
    if ( status != 0 ) {
        bson_sprintf( errstr, "getaddrinfo failed with error %d", status );
        __mongo_set_error( conn, MONGO_CONN_ADDR_FAIL, errstr, WSAGetLastError() );
        return MONGO_ERROR;
    }

    for ( ai_ptr = ai_list; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next ) {
        conn->sock = socket( ai_ptr->ai_family, ai_ptr->ai_socktype,
                             ai_ptr->ai_protocol );

        if ( conn->sock < 0 ) {
            __mongo_set_error( conn, MONGO_SOCKET_ERROR, "socket() failed",
                               WSAGetLastError() );
            conn->sock = 0;
            continue;
        }

        status = connect( conn->sock, ai_ptr->ai_addr, ai_ptr->ai_addrlen );
        if ( status != 0 ) {
            __mongo_set_error( conn, MONGO_SOCKET_ERROR, "connect() failed",
                               WSAGetLastError() );
            mongo_env_close_socket( conn->sock );
            conn->sock = 0;
            continue;
        }

        if ( ai_ptr->ai_protocol == IPPROTO_TCP ) {
            int flag = 1;

            setsockopt( conn->sock, IPPROTO_TCP, TCP_NODELAY,
                        ( void * ) &flag, sizeof( flag ) );

            if ( conn->op_timeout_ms > 0 )
                mongo_env_set_socket_op_timeout( conn, conn->op_timeout_ms );
        }

        conn->connected = 1;
        break;
    }

    freeaddrinfo( ai_list );

    if ( ! conn->connected ) {
        conn->err = MONGO_CONN_FAIL;
        return MONGO_ERROR;
    }
    else {
        mongo_clear_errors( conn );
        return MONGO_OK;
    }
}