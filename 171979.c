MONGO_EXPORT bson_bool_t mongo_cmd_ismaster( mongo *conn, bson *realout ) {
    bson out = {NULL,0};
    bson_bool_t ismaster = 0;

    if ( mongo_simple_int_command( conn, "admin", "ismaster", 1, &out ) == MONGO_OK ) {
        bson_iterator it;
        bson_find( &it, &out, "ismaster" );
        ismaster = bson_iterator_bool( &it );
    }

    if( realout )
        *realout = out; /* transfer of ownership */
    else
        bson_destroy( &out );

    return ismaster;
}