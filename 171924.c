MONGO_EXPORT double mongo_count( mongo *conn, const char *db, const char *ns, const bson *query ) {
    bson cmd;
    bson out = {NULL, 0};
    double count = -1;

    bson_init( &cmd );
    bson_append_string( &cmd, "count", ns );
    if ( query && bson_size( query ) > 5 ) /* not empty */
        bson_append_bson( &cmd, "query", query );
    bson_finish( &cmd );

    if( mongo_run_command( conn, db, &cmd, &out ) == MONGO_OK ) {
        bson_iterator it;
        if( bson_find( &it, &out, "n" ) )
            count = bson_iterator_double( &it );
        bson_destroy( &cmd );
        bson_destroy( &out );
        return count;
    }
    else {
        bson_destroy( &out );
        bson_destroy( &cmd );
        return MONGO_ERROR;
    }
}