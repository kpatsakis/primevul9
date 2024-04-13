static void bson_append_byte( bson *b, char c ) {
    b->cur[0] = c;
    b->cur++;
}