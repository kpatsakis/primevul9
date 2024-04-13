MONGO_EXPORT gridfs* gridfs_create( void ) {
    return (gridfs*)bson_malloc(sizeof(gridfs));
}