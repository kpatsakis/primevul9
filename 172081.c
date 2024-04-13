MONGO_EXPORT mongo_cursor* mongo_cursor_create( void ) {
    return (mongo_cursor*)bson_malloc(sizeof(mongo_cursor));
}