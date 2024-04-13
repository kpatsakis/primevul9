MONGO_EXPORT int bson_append_string( bson *b, const char *name, const char *value ) {
    return bson_append_string_base( b, name, value, strlen ( value ), BSON_STRING );
}