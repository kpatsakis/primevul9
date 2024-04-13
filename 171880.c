MONGO_EXPORT void mongo_write_concern_init( mongo_write_concern *write_concern ) {
    memset( write_concern, 0, sizeof( mongo_write_concern ) );
}