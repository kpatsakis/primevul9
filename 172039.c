MONGO_EXPORT void bson_destroy( bson *b ) {
    if (b) {
        bson_free( b->data );
        b->err = 0;
        b->data = 0;
        b->cur = 0;
        b->finished = 1;
    }
}