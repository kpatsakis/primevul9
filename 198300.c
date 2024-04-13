Mat_WriteCompressedEmptyVariable5(mat_t *mat,const char *name,int rank,
                                  size_t *dims,z_streamp z)
{
    mat_uint32_t array_flags;
    int array_flags_type = MAT_T_UINT32, dims_array_type = MAT_T_INT32;
    int array_flags_size = 8;
    int i, err;
    size_t nBytes, empty_matrix_max_buf_size;

    mat_uint32_t comp_buf[512];
    mat_uint32_t uncomp_buf[512] = {0,};
    int buf_size = 512;
    size_t byteswritten = 0, buf_size_bytes;

    if ( NULL == mat || NULL == z)
        return byteswritten;

    buf_size_bytes = buf_size*sizeof(*comp_buf);

    /* Array Flags */
    array_flags = MAT_C_DOUBLE;

    uncomp_buf[0] = MAT_T_MATRIX;
    err = GetEmptyMatrixMaxBufSize(name, rank, &empty_matrix_max_buf_size);
    if (err || empty_matrix_max_buf_size > UINT32_MAX)
        return byteswritten;
    uncomp_buf[1] = empty_matrix_max_buf_size;
    z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
    z->avail_in = 8;
    do {
        z->next_out  = ZLIB_BYTE_PTR(comp_buf);
        z->avail_out = buf_size_bytes;
        deflate(z,Z_NO_FLUSH);
        byteswritten += fwrite(comp_buf,1,buf_size_bytes-z->avail_out,(FILE*)mat->fp);
    } while ( z->avail_out == 0 );
    uncomp_buf[0] = array_flags_type;
    uncomp_buf[1] = array_flags_size;
    uncomp_buf[2] = array_flags;
    uncomp_buf[3] = 0;
    /* Rank and Dimension */
    nBytes = rank * 4;
    uncomp_buf[4] = dims_array_type;
    uncomp_buf[5] = nBytes;
    for ( i = 0; i < rank; i++ ) {
        mat_int32_t dim;
        dim = dims[i];
        uncomp_buf[6+i] = dim;
    }
    if ( rank % 2 != 0 ) {
        int pad4 = 0;
        uncomp_buf[6+i] = pad4;
        i++;
    }

    z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
    z->avail_in = (6+i)*sizeof(*uncomp_buf);
    do {
        z->next_out  = ZLIB_BYTE_PTR(comp_buf);
        z->avail_out = buf_size_bytes;
        deflate(z,Z_NO_FLUSH);
        byteswritten += fwrite(comp_buf,1,buf_size_bytes-z->avail_out,(FILE*)mat->fp);
    } while ( z->avail_out == 0 );
    /* Name of variable */
    if ( NULL == name ) {
        mat_int16_t array_name_type = MAT_T_INT8;
        uncomp_buf[0] = array_name_type;
        uncomp_buf[1] = 0;
        z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
        z->avail_in = 8;
        do {
            z->next_out  = ZLIB_BYTE_PTR(comp_buf);
            z->avail_out = buf_size_bytes;
            deflate(z,Z_NO_FLUSH);
            byteswritten += fwrite(comp_buf,1,buf_size_bytes-z->avail_out,(FILE*)mat->fp);
        } while ( z->avail_out == 0 );
    } else if ( strlen(name) <= 4 ) {
        mat_int16_t array_name_len = (mat_int16_t)strlen(name);
        mat_int16_t array_name_type = MAT_T_INT8;

        memset(uncomp_buf,0,8);
        uncomp_buf[0] = (array_name_len << 16) | array_name_type;
        memcpy(uncomp_buf+1,name,array_name_len);
        if ( array_name_len % 4 )
            array_name_len += 4-(array_name_len % 4);

        z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
        z->avail_in = 8;
        do {
            z->next_out  = ZLIB_BYTE_PTR(comp_buf);
            z->avail_out = buf_size_bytes;
            deflate(z,Z_NO_FLUSH);
            byteswritten += fwrite(comp_buf,1,buf_size_bytes-z->avail_out,
                (FILE*)mat->fp);
        } while ( z->avail_out == 0 );
    } else {
        mat_int32_t array_name_len = (mat_int32_t)strlen(name);
        mat_int32_t array_name_type = MAT_T_INT8;

        memset(uncomp_buf,0,buf_size*sizeof(*uncomp_buf));
        uncomp_buf[0] = array_name_type;
        uncomp_buf[1] = array_name_len;
        memcpy(uncomp_buf+2,name,array_name_len);
        if ( array_name_len % 8 )
            array_name_len += 8-(array_name_len % 8);
        z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
        z->avail_in = 8+array_name_len;
        do {
            z->next_out  = ZLIB_BYTE_PTR(comp_buf);
            z->avail_out = buf_size_bytes;
            deflate(z,Z_NO_FLUSH);
            byteswritten += fwrite(comp_buf,1,buf_size_bytes-z->avail_out,
                (FILE*)mat->fp);
        } while ( z->avail_out == 0 );
    }

    byteswritten += WriteCompressedData(mat,z,NULL,0,MAT_T_DOUBLE);
    return byteswritten;
}