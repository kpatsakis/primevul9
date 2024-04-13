MONGO_EXPORT int mongo_insert( mongo *conn, const char *ns,
                               const bson *bson, mongo_write_concern *custom_write_concern ) {

    char *data;
    mongo_message *mm;
    mongo_write_concern *write_concern = NULL;

    if( mongo_validate_ns( conn, ns ) != MONGO_OK )
        return MONGO_ERROR;

    if( mongo_bson_valid( conn, bson, 1 ) != MONGO_OK ) {
        return MONGO_ERROR;
    }

    if( mongo_choose_write_concern( conn, custom_write_concern,
                                    &write_concern ) == MONGO_ERROR ) {
        return MONGO_ERROR;
    }

    mm = mongo_message_create( 16 /* header */
                               + 4 /* ZERO */
                               + strlen( ns )
                               + 1 + bson_size( bson )
                               , 0, 0, MONGO_OP_INSERT );

    data = &mm->data;
    data = mongo_data_append32( data, &ZERO );
    data = mongo_data_append( data, ns, strlen( ns ) + 1 );
    mongo_data_append( data, bson->data, bson_size( bson ) );


    /* TODO: refactor so that we can send the insert message
       and the getlasterror messages together. */
    if( write_concern ) {
        if( mongo_message_send( conn, mm ) == MONGO_ERROR ) {
            return MONGO_ERROR;
        }

        return mongo_check_last_error( conn, ns, write_concern );
    }
    else {
        return mongo_message_send( conn, mm );
    }
}