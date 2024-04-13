MONGO_EXPORT void bson_iterator_dispose(bson_iterator* i) {
    free(i);
}