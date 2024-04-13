MONGO_EXPORT void mongo_write_concern_destroy( mongo_write_concern *write_concern ) {
    if( !write_concern )
        return;

    if( write_concern->cmd )
        bson_destroy( write_concern->cmd );

    bson_free( write_concern->cmd );
}