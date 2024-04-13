MONGO_EXPORT int bson_append_time_t( bson *b, const char *name, time_t secs ) {
    return bson_append_date( b, name, ( bson_date_t )secs * 1000 );
}