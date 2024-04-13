MONGO_EXPORT mongo_cursor *mongo_find( mongo *conn, const char *ns, const bson *query,
                                       const bson *fields, int limit, int skip, int options ) {

    mongo_cursor *cursor = ( mongo_cursor * )bson_malloc( sizeof( mongo_cursor ) );
    mongo_cursor_init( cursor, conn, ns );
    cursor->flags |= MONGO_CURSOR_MUST_FREE;

    mongo_cursor_set_query( cursor, query );
    mongo_cursor_set_fields( cursor, fields );
    mongo_cursor_set_limit( cursor, limit );
    mongo_cursor_set_skip( cursor, skip );
    mongo_cursor_set_options( cursor, options );

    if( mongo_cursor_op_query( cursor ) == MONGO_OK )
        return cursor;
    else {
        mongo_cursor_destroy( cursor );
        return NULL;
    }
}