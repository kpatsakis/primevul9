static int mongo_replica_set_check_host( mongo *conn ) {

    bson out;
    bson_iterator it;
    bson_bool_t ismaster = 0;
    const char *set_name;
    int max_bson_size = MONGO_DEFAULT_MAX_BSON_SIZE;

    out.data = NULL;

    if ( mongo_simple_int_command( conn, "admin", "ismaster", 1, &out ) == MONGO_OK ) {
        if( bson_find( &it, &out, "ismaster" ) )
            ismaster = bson_iterator_bool( &it );

        if( bson_find( &it, &out, "maxBsonObjectSize" ) )
            max_bson_size = bson_iterator_int( &it );
        conn->max_bson_size = max_bson_size;

        if( bson_find( &it, &out, "setName" ) ) {
            set_name = bson_iterator_string( &it );
            if( strcmp( set_name, conn->replica_set->name ) != 0 ) {
                bson_destroy( &out );
                conn->err = MONGO_CONN_BAD_SET_NAME;
                return MONGO_ERROR;
            }
        }
    }

    bson_destroy( &out );

    if( ismaster ) {
        conn->replica_set->primary_connected = 1;
    }
    else {
        mongo_env_close_socket( conn->sock );
    }

    return MONGO_OK;
}