MONGO_EXPORT int mongo_remove( mongo *conn, const char *ns, const bson *cond,
                               mongo_write_concern *custom_write_concern ) {

    char *data;
    mongo_message *mm;
    mongo_write_concern *write_concern = NULL;

    /* Make sure that the BSON is valid UTF-8.
     * TODO: decide whether to check cond as well.
     * */
    if( mongo_bson_valid( conn, ( bson * )cond, 0 ) != MONGO_OK ) {
        return MONGO_ERROR;
    }

    if( mongo_choose_write_concern( conn, custom_write_concern,
                                    &write_concern ) == MONGO_ERROR ) {
        return MONGO_ERROR;
    }

    mm = mongo_message_create( 16  /* header */
                               + 4  /* ZERO */
                               + strlen( ns ) + 1
                               + 4  /* ZERO */
                               + bson_size( cond )
                               , 0 , 0 , MONGO_OP_DELETE );

    data = &mm->data;
    data = mongo_data_append32( data, &ZERO );
    data = mongo_data_append( data, ns, strlen( ns ) + 1 );
    data = mongo_data_append32( data, &ZERO );
    mongo_data_append( data, cond->data, bson_size( cond ) );

    /* TODO: refactor so that we can send the insert message
     * and the getlasterror messages together. */
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