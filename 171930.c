MONGO_EXPORT gridfile* gridfile_create( void ) {
    return (gridfile*)bson_malloc(sizeof(gridfile));
}