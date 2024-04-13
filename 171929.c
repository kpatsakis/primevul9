static char *mongo_data_append32( char *start , const void *data ) {
    bson_little_endian32( start , data );
    return start + 4;
}