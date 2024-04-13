static void mongo_replica_set_add_node( mongo_host_port **list, const char *host, int port ) {
    mongo_host_port *host_port = bson_malloc( sizeof( mongo_host_port ) );
    host_port->port = port;
    host_port->next = NULL;
    strncpy( host_port->host, host, strlen( host ) + 1 );

    if( *list == NULL )
        *list = host_port;
    else {
        mongo_host_port *p = *list;
        while( p->next != NULL )
            p = p->next;
        p->next = host_port;
    }
}