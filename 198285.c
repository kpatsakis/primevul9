Mat_WriteEmptyVariable5(mat_t *mat,const char *name,int rank,size_t *dims)
{
    mat_uint32_t array_flags;
    mat_int32_t  array_name_type = MAT_T_INT8, matrix_type = MAT_T_MATRIX;
    int array_flags_type = MAT_T_UINT32, dims_array_type = MAT_T_INT32;
    int array_flags_size = 8, pad4 = 0, nBytes, i;
    const mat_int8_t pad1 = 0;
    size_t byteswritten = 0;
    long start = 0, end = 0;

    fwrite(&matrix_type,4,1,(FILE*)mat->fp);
    fwrite(&pad4,4,1,(FILE*)mat->fp);
    start = ftell((FILE*)mat->fp);

    /* Array Flags */
    array_flags = MAT_C_DOUBLE;

    if ( mat->byteswap )
        array_flags = Mat_int32Swap((mat_int32_t*)&array_flags);
    byteswritten += fwrite(&array_flags_type,4,1,(FILE*)mat->fp);
    byteswritten += fwrite(&array_flags_size,4,1,(FILE*)mat->fp);
    byteswritten += fwrite(&array_flags,4,1,(FILE*)mat->fp);
    byteswritten += fwrite(&pad4,4,1,(FILE*)mat->fp);
    /* Rank and Dimension */
    nBytes = rank * 4;
    byteswritten += fwrite(&dims_array_type,4,1,(FILE*)mat->fp);
    byteswritten += fwrite(&nBytes,4,1,(FILE*)mat->fp);
    for ( i = 0; i < rank; i++ ) {
        mat_int32_t dim;
        dim = dims[i];
        byteswritten += fwrite(&dim,4,1,(FILE*)mat->fp);
    }
    if ( rank % 2 != 0 )
        byteswritten += fwrite(&pad4,4,1,(FILE*)mat->fp);

    if ( NULL == name ) {
        /* Name of variable */
        byteswritten += fwrite(&array_name_type,4,1,(FILE*)mat->fp);
        byteswritten += fwrite(&pad4,4,1,(FILE*)mat->fp);
    } else {
        mat_int32_t array_name_len = (mat_int32_t)strlen(name);
        /* Name of variable */
        if ( array_name_len <= 4 ) {
            array_name_type = (array_name_len << 16) | array_name_type;
            byteswritten += fwrite(&array_name_type,4,1,(FILE*)mat->fp);
            byteswritten += fwrite(name,1,array_name_len,(FILE*)mat->fp);
            for ( i = array_name_len; i < 4; i++ )
                byteswritten += fwrite(&pad1,1,1,(FILE*)mat->fp);
        } else {
            byteswritten += fwrite(&array_name_type,4,1,(FILE*)mat->fp);
            byteswritten += fwrite(&array_name_len,4,1,(FILE*)mat->fp);
            byteswritten += fwrite(name,1,array_name_len,(FILE*)mat->fp);
            if ( array_name_len % 8 )
                for ( i = array_name_len % 8; i < 8; i++ )
                    byteswritten += fwrite(&pad1,1,1,(FILE*)mat->fp);
        }
    }

    nBytes = WriteData(mat,NULL,0,MAT_T_DOUBLE);
    byteswritten += nBytes;
    if ( nBytes % 8 )
        for ( i = nBytes % 8; i < 8; i++ )
            byteswritten += fwrite(&pad1,1,1,(FILE*)mat->fp);

    end = ftell((FILE*)mat->fp);
    if ( start != -1L && end != -1L ) {
        nBytes = (int)(end-start);
        (void)fseek((FILE*)mat->fp,(long)-(nBytes+4),SEEK_CUR);
        fwrite(&nBytes,4,1,(FILE*)mat->fp);
        (void)fseek((FILE*)mat->fp,end,SEEK_SET);
    } else {
        Mat_Critical("Couldn't determine file position");
    }

    return byteswritten;
}