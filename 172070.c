int mongo_env_set_socket_op_timeout( mongo *conn, int millis ) {
    struct timeval tv;
    tv.tv_sec = millis / 1000;
    tv.tv_usec = ( millis % 1000 ) * 1000;

    if ( setsockopt( conn->sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof( tv ) ) == -1 ) {
        conn->err = MONGO_IO_ERROR;
        __mongo_set_error( conn, MONGO_IO_ERROR, "setsockopt SO_RCVTIMEO failed.", errno );
        return MONGO_ERROR;
    }

    if ( setsockopt( conn->sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof( tv ) ) == -1 ) {
        __mongo_set_error( conn, MONGO_IO_ERROR, "setsockopt SO_SNDTIMEO failed.", errno );
        return MONGO_ERROR;
    }

    return MONGO_OK;
}