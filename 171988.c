MONGO_EXPORT int mongo_cursor_next( mongo_cursor *cursor ) {
    char *next_object;
    char *message_end;

    if( ! ( cursor->flags & MONGO_CURSOR_QUERY_SENT ) )
        if( mongo_cursor_op_query( cursor ) != MONGO_OK )
            return MONGO_ERROR;

    if( !cursor->reply )
        return MONGO_ERROR;

    /* no data */
    if ( cursor->reply->fields.num == 0 ) {

        /* Special case for tailable cursors. */
        if( cursor->reply->fields.cursorID ) {
            if( ( mongo_cursor_get_more( cursor ) != MONGO_OK ) ||
                    cursor->reply->fields.num == 0 ) {
                return MONGO_ERROR;
            }
        }

        else
            return MONGO_ERROR;
    }

    /* first */
    if ( cursor->current.data == NULL ) {
        bson_init_finished_data( &cursor->current, &cursor->reply->objs );
        return MONGO_OK;
    }

    next_object = cursor->current.data + bson_size( &cursor->current );
    message_end = ( char * )cursor->reply + cursor->reply->head.len;

    if ( next_object >= message_end ) {
        if( mongo_cursor_get_more( cursor ) != MONGO_OK )
            return MONGO_ERROR;

        if ( cursor->reply->fields.num == 0 ) {
            /* Special case for tailable cursors. */
            if ( cursor->reply->fields.cursorID ) {
                cursor->err = MONGO_CURSOR_PENDING;
                return MONGO_ERROR;
            }
            else
                return MONGO_ERROR;
        }

        bson_init_finished_data( &cursor->current, &cursor->reply->objs );
    }
    else {
        bson_init_finished_data( &cursor->current, next_object );
    }

    return MONGO_OK;
}