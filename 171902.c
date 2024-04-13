MONGO_EXPORT gridfs_offset gridfile_seek( gridfile *gfile, gridfs_offset offset ) {
    gridfs_offset length;

    length = gridfile_get_contentlength( gfile );
    gfile->pos = length < offset ? length : offset;
    return gfile->pos;
}