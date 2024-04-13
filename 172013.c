int gridfile_init( gridfs *gfs, bson *meta, gridfile *gfile )

{
    gfile->gfs = gfs;
    gfile->pos = 0;
    gfile->meta = ( bson * )bson_malloc( sizeof( bson ) );
    if ( gfile->meta == NULL ) return MONGO_ERROR;
    bson_copy( gfile->meta, meta );
    return MONGO_OK;
}