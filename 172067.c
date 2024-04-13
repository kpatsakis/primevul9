MONGO_EXPORT int mongo_find_one( mongo *conn, const char *ns, const bson *query,
                                 const bson *fields, bson *out ) {

    mongo_cursor cursor[1];
    mongo_cursor_init( cursor, conn, ns );
    mongo_cursor_set_query( cursor, query );
    mongo_cursor_set_fields( cursor, fields );
    mongo_cursor_set_limit( cursor, 1 );

    if ( mongo_cursor_next( cursor ) == MONGO_OK ) {
        if( out ) {
            bson_init_size( out, bson_size( (bson *)&cursor->current ) );
            memcpy( out->data, cursor->current.data,
                    bson_size( (bson *)&cursor->current ) );
            out->finished = 1;
        }
        mongo_cursor_destroy( cursor );
        return MONGO_OK;
    }
    else {
        mongo_cursor_destroy( cursor );
        return MONGO_ERROR;
    }
}