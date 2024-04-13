MONGO_EXPORT void mongo_cursor_init( mongo_cursor *cursor, mongo *conn, const char *ns ) {
    memset( cursor, 0, sizeof( mongo_cursor ) );
    cursor->conn = conn;
    cursor->ns = ( const char * )bson_malloc( strlen( ns ) + 1 );
    strncpy( ( char * )cursor->ns, ns, strlen( ns ) + 1 );
    cursor->current.data = NULL;
}