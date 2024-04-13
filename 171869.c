MONGO_EXPORT void gridfile_get_chunk( gridfile *gfile, int n, bson* out ) {
    bson query;

    bson_iterator it;
    bson_oid_t id;
    int result;

    bson_init( &query );
    bson_find( &it, gfile->meta, "_id" );
    id = *bson_iterator_oid( &it );
    bson_append_oid( &query, "files_id", &id );
    bson_append_int( &query, "n", n );
    bson_finish( &query );

    result = (mongo_find_one(gfile->gfs->client,
                             gfile->gfs->chunks_ns,
                             &query, NULL, out ) == MONGO_OK );
    bson_destroy( &query );
    if (!result) {
        bson empty;
        bson_empty(&empty);
        bson_copy(out, &empty);
    }
}