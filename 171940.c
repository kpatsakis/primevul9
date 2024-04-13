MONGO_EXPORT void mongo_init( mongo *conn ) {
    memset( conn, 0, sizeof( mongo ) );
    conn->max_bson_size = MONGO_DEFAULT_MAX_BSON_SIZE;
    mongo_set_write_concern( conn, &WC1 );
}