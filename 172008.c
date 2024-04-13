MONGO_EXPORT void bson_set_oid_fuzz( int ( *func )( void ) ) {
    oid_fuzz_func = func;
}