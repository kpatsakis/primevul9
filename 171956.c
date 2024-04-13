MONGO_EXPORT int mongo_client( mongo *conn , const char *host, int port ) {
    mongo_init( conn );

    conn->primary = bson_malloc( sizeof( mongo_host_port ) );
    strncpy( conn->primary->host, host, strlen( host ) + 1 );
    conn->primary->port = port;
    conn->primary->next = NULL;

    if( mongo_env_socket_connect( conn, host, port ) != MONGO_OK )
        return MONGO_ERROR;

    if( mongo_check_is_master( conn ) != MONGO_OK )
        return MONGO_ERROR;
    else
        return MONGO_OK;
}