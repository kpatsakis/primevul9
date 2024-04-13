static void _bson_reset( bson *b ) {
    b->finished = 0;
    b->stackPos = 0;
    b->err = 0;
    b->errstr = NULL;
}