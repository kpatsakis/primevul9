int bson_init_finished_data( bson *b, char *data ) {
    bson_init_data( b, data );
    b->finished = 1;
    return BSON_OK;
}