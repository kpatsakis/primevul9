void mongo_parse_host( const char *host_string, mongo_host_port *host_port ) {
    int len, idx, split;
    len = split = idx = 0;

    /* Split the host_port string at the ':' */
    while( 1 ) {
        if( *( host_string + len ) == '\0' )
            break;
        if( *( host_string + len ) == ':' )
            split = len;

        len++;
    }

    /* If 'split' is set, we know the that port exists;
     * Otherwise, we set the default port. */
    idx = split ? split : len;
    memcpy( host_port->host, host_string, idx );
    memcpy( host_port->host + idx, "\0", 1 );
    if( split )
        host_port->port = atoi( host_string + idx + 1 );
    else
        host_port->port = MONGO_DEFAULT_PORT;
}