MONGO_EXPORT int mongo_check_connection( mongo *conn ) {
    if( ! conn->connected )
        return MONGO_ERROR;

    if( mongo_simple_int_command( conn, "admin", "ping", 1, NULL ) == MONGO_OK )
        return MONGO_OK;
    else
        return MONGO_ERROR;
}