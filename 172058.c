static void bson_append64( bson *b, const void *data ) {
    bson_little_endian64( b->cur, data );
    b->cur += 8;
}