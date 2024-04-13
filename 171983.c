MONGO_EXPORT void bson_iterator_from_buffer( bson_iterator *i, const char *buffer ) {
    i->cur = buffer + 4;
    i->first = 1;
}