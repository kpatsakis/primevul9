int mongo_env_read_socket( mongo *conn, void *buf, size_t len ) {
    char *cbuf = buf;
    while ( len ) {
        size_t sent = recv( conn->sock, cbuf, len, 0 );
        if ( sent == 0 || sent == -1 ) {
            conn->err = MONGO_IO_ERROR;
            return MONGO_ERROR;
        }
        cbuf += sent;
        len -= sent;
    }

    return MONGO_OK;
}