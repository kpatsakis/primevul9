int bson_init_data( bson *b, char *data ) {
    b->data = data;
    return BSON_OK;
}