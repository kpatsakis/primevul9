MONGO_EXPORT int gridfile_writer_done( gridfile *gfile ) {

    /* write any remaining pending chunk data.
     * pending data will always take up less than one chunk */
    bson *oChunk;
    int response;
    if( gfile->pending_data ) {
        oChunk = chunk_new( gfile->id, gfile->chunk_num, gfile->pending_data, gfile->pending_len );
        mongo_insert( gfile->gfs->client, gfile->gfs->chunks_ns, oChunk, NULL );
        chunk_free( oChunk );
        bson_free( gfile->pending_data );
        gfile->length += gfile->pending_len;
    }

    /* insert into files collection */
    response = gridfs_insert_file( gfile->gfs, gfile->remote_name, gfile->id,
                                   gfile->length, gfile->content_type );

    bson_free( gfile->remote_name );
    bson_free( gfile->content_type );

    return response;
}