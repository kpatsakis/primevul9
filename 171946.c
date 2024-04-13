MONGO_EXPORT void bson_set_oid_inc( int ( *func )( void ) ) {
    oid_inc_func = func;
}