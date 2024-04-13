MONGO_EXPORT mongo* mongo_create( void ) {
    return (mongo*)bson_malloc(sizeof(mongo));
}