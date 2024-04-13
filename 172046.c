MONGO_EXPORT bson* bson_create( void ) {
    return (bson*)bson_malloc(sizeof(bson));
}