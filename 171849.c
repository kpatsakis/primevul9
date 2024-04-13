MONGO_EXPORT const char *bson_data( const bson *b ) {
    return (const char *)b->data;
}