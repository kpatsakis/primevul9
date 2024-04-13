MONGO_EXPORT void gridfile_destroy( gridfile *gfile )

{
    bson_destroy( gfile->meta );
    bson_free( gfile->meta );
}