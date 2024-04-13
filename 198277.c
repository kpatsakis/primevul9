WriteCompressedData(mat_t *mat,z_streamp z,void *data,int N,
    enum matio_types data_type)
{
    int nBytes = 0, data_size, data_tag[2], byteswritten = 0;
    int buf_size = 1024;
    mat_uint8_t buf[1024], pad[8] = {0,};

    if ( mat == NULL || mat->fp == NULL )
        return 0;

    data_size   = Mat_SizeOf(data_type);
    data_tag[0] = data_type;
    data_tag[1] = data_size*N;
    z->next_in  = ZLIB_BYTE_PTR(data_tag);
    z->avail_in = 8;
    do {
        z->next_out  = buf;
        z->avail_out = buf_size;
        deflate(z,Z_NO_FLUSH);
        byteswritten += fwrite(buf,1,buf_size-z->avail_out,(FILE*)mat->fp);
    } while ( z->avail_out == 0 );

    /* exit early if this is an empty data */
    if ( NULL == data || N < 1 )
        return byteswritten;

    z->next_in  = (Bytef*)data;
    z->avail_in = N*data_size;
    do {
        z->next_out  = buf;
        z->avail_out = buf_size;
        deflate(z,Z_NO_FLUSH);
        byteswritten += fwrite(buf,1,buf_size-z->avail_out,(FILE*)mat->fp);
    } while ( z->avail_out == 0 );
    /* Add/Compress padding to pad to 8-byte boundary */
    if ( N*data_size % 8 ) {
        z->next_in  = pad;
        z->avail_in = 8 - (N*data_size % 8);
        do {
            z->next_out  = buf;
            z->avail_out = buf_size;
            deflate(z,Z_NO_FLUSH);
            byteswritten += fwrite(buf,1,buf_size-z->avail_out,(FILE*)mat->fp);
        } while ( z->avail_out == 0 );
    }
    nBytes = byteswritten;
    return nBytes;
}