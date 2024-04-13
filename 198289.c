WriteCompressedCellArrayField(mat_t *mat,matvar_t *matvar,z_streamp z)
{
    mat_uint32_t comp_buf[512];
    mat_uint32_t uncomp_buf[512] = {0,};
    int buf_size = 512;
    size_t byteswritten = 0, field_buf_size;

    if ( NULL == matvar || NULL == mat || NULL == z)
        return 0;

    uncomp_buf[0] = MAT_T_MATRIX;
    if ( MAT_C_EMPTY != matvar->class_type ) {
        int err = GetCellArrayFieldBufSize(matvar, &field_buf_size);
        if (err || field_buf_size > UINT32_MAX)
            return 0;

        uncomp_buf[1] = field_buf_size;
    } else {
        uncomp_buf[1] = 0;
    }
    z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
    z->avail_in = 8;
    do {
        z->next_out  = ZLIB_BYTE_PTR(comp_buf);
        z->avail_out = buf_size*sizeof(*comp_buf);
        deflate(z,Z_NO_FLUSH);
        byteswritten += fwrite(comp_buf,1,buf_size*sizeof(*comp_buf)-z->avail_out,
            (FILE*)mat->fp);
    } while ( z->avail_out == 0 );

    byteswritten += WriteCompressedTypeArrayFlags(mat,matvar,z);
    return byteswritten;
}