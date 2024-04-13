MONGO_EXPORT int mongo_write_concern_finish( mongo_write_concern *write_concern ) {
    bson *command;

    /* Destory any existing serialized write concern object and reuse it. */
    if( write_concern->cmd ) {
        bson_destroy( write_concern->cmd );
        command = write_concern->cmd;
    }
    else
        command = (bson *)bson_malloc( sizeof( bson ) );

    if( !command ) {
        return MONGO_ERROR;
    }

    bson_init( command );

    bson_append_int( command, "getlasterror", 1 );

    if( write_concern->mode ) {
        bson_append_string( command, "w", write_concern->mode );
    }

    else if( write_concern->w && write_concern->w > 1 ) {
        bson_append_int( command, "w", write_concern->w );
    }

    if( write_concern->wtimeout ) {
        bson_append_int( command, "wtimeout", write_concern->wtimeout );
    }

    if( write_concern->j ) {
        bson_append_int( command, "j", write_concern->j );
    }

    if( write_concern->fsync ) {
        bson_append_int( command, "fsync", write_concern->fsync );
    }

    bson_finish( command );

    /* write_concern now owns the BSON command object.
     * This is freed in mongo_write_concern_destroy(). */
    write_concern->cmd = command;

    return MONGO_OK;
}