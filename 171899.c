MONGO_EXPORT int bson_append_new_oid( bson *b, const char *name ) {
    bson_oid_t oid;
    bson_oid_gen( &oid );
    return bson_append_oid( b, name, &oid );
}