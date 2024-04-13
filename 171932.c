static void mongo_replica_set_check_seed( mongo *conn ) {
    bson out;
    bson hosts;
    const char *data;
    bson_iterator it;
    bson_iterator it_sub;
    const char *host_string;
    mongo_host_port *host_port = NULL;

    out.data = NULL;

    hosts.data = NULL;

    if( mongo_simple_int_command( conn, "admin", "ismaster", 1, &out ) == MONGO_OK ) {

        if( bson_find( &it, &out, "hosts" ) ) {
            data = bson_iterator_value( &it );
            bson_iterator_from_buffer( &it_sub, data );

            /* Iterate over host list, adding each host to the
             * connection's host list. */
            while( bson_iterator_next( &it_sub ) ) {
                host_string = bson_iterator_string( &it_sub );

                host_port = bson_malloc( sizeof( mongo_host_port ) );

                if( host_port ) {
                    mongo_parse_host( host_string, host_port );
                    mongo_replica_set_add_node( &conn->replica_set->hosts,
                                                host_port->host, host_port->port );

                    bson_free( host_port );
                    host_port = NULL;
                }
            }
        }
    }

    bson_destroy( &out );
    bson_destroy( &hosts );
    mongo_env_close_socket( conn->sock );
    conn->sock = 0;
    conn->connected = 0;

}