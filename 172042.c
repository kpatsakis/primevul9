static int mongo_check_is_master( mongo *conn ) {
    bson out;
    bson_iterator it;
    bson_bool_t ismaster = 0;
    int max_bson_size = MONGO_DEFAULT_MAX_BSON_SIZE;

    out.data = NULL;

    if ( mongo_simple_int_command( conn, "admin", "ismaster", 1, &out ) == MONGO_OK ) {
        if( bson_find( &it, &out, "ismaster" ) )
            ismaster = bson_iterator_bool( &it );
        if( bson_find( &it, &out, "maxBsonObjectSize" ) ) {
            max_bson_size = bson_iterator_int( &it );
        }
        conn->max_bson_size = max_bson_size;
    }
    else {
        return MONGO_ERROR;
    }

    bson_destroy( &out );

    if( ismaster )
        return MONGO_OK;
    else {
        conn->err = MONGO_CONN_NOT_MASTER;
        return MONGO_ERROR;
    }
}