int mongo_env_set_socket_op_timeout( mongo *conn, int millis ) {
    if ( setsockopt( conn->sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&millis,
                     sizeof( millis ) ) == -1 ) {
        __mongo_set_error( conn, MONGO_IO_ERROR, "setsockopt SO_RCVTIMEO failed.",
                           WSAGetLastError() );
        return MONGO_ERROR;
    }

    if ( setsockopt( conn->sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&millis,
                     sizeof( millis ) ) == -1 ) {
        __mongo_set_error( conn, MONGO_IO_ERROR, "setsockopt SO_SNDTIMEO failed.",
                           WSAGetLastError() );
        return MONGO_ERROR;
    }

    return MONGO_OK;
}