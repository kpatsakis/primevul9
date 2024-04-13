MONGO_EXPORT void bson_iterator_init( bson_iterator *i, const bson *b ) {
    i->cur = b->data + 4;
    i->first = 1;
}