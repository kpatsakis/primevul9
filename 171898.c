MONGO_EXPORT int mongo_cursor_destroy( mongo_cursor *cursor ) {
    int result = MONGO_OK;

    if ( !cursor ) return result;

    /* Kill cursor if live. */
    if ( cursor->reply && cursor->reply->fields.cursorID ) {
        mongo *conn = cursor->conn;
        mongo_message *mm = mongo_message_create( 16 /*header*/
                            +4 /*ZERO*/
                            +4 /*numCursors*/
                            +8 /*cursorID*/
                            , 0, 0, MONGO_OP_KILL_CURSORS );
        char *data = &mm->data;
        data = mongo_data_append32( data, &ZERO );
        data = mongo_data_append32( data, &ONE );
        mongo_data_append64( data, &cursor->reply->fields.cursorID );

        result = mongo_message_send( conn, mm );
    }

    bson_free( cursor->reply );
    bson_free( ( void * )cursor->ns );

    if( cursor->flags & MONGO_CURSOR_MUST_FREE )
        bson_free( cursor );

    return result;
}