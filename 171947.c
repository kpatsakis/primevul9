static void bson_append32( bson *b, const void *data ) {
    bson_little_endian32( b->cur, data );
    b->cur += 4;
}