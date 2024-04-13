MONGO_EXPORT int mongo_replica_set_client( mongo *conn ) {

    int res = 0;
    mongo_host_port *node;

    conn->sock = 0;
    conn->connected = 0;

    /* First iterate over the seed nodes to get the canonical list of hosts
     * from the replica set. Break out once we have a host list.
     */
    node = conn->replica_set->seeds;
    while( node != NULL ) {
        res = mongo_env_socket_connect( conn, ( const char * )&node->host, node->port );
        if( res == MONGO_OK ) {
            mongo_replica_set_check_seed( conn );
            if( conn->replica_set->hosts )
                break;
        }
        node = node->next;
    }

    /* Iterate over the host list, checking for the primary node. */
    if( !conn->replica_set->hosts ) {
        conn->err = MONGO_CONN_NO_PRIMARY;
        return MONGO_ERROR;
    }
    else {
        node = conn->replica_set->hosts;

        while( node != NULL ) {
            res = mongo_env_socket_connect( conn, ( const char * )&node->host, node->port );

            if( res == MONGO_OK ) {
                if( mongo_replica_set_check_host( conn ) != MONGO_OK )
                    return MONGO_ERROR;

                /* Primary found, so return. */
                else if( conn->replica_set->primary_connected ) {
                    conn->primary = bson_malloc( sizeof( mongo_host_port ) );
                    strncpy( conn->primary->host, node->host, strlen( node->host ) + 1 );
                    conn->primary->port = node->port;
                    return MONGO_OK;
                }

                /* No primary, so close the connection. */
                else {
                    mongo_env_close_socket( conn->sock );
                    conn->sock = 0;
                    conn->connected = 0;
                }
            }

            node = node->next;
        }
    }


    conn->err = MONGO_CONN_NO_PRIMARY;
    return MONGO_ERROR;
}