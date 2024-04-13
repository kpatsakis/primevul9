WriteCompressedTypeArrayFlags(mat_t *mat,matvar_t *matvar,z_streamp z)
{
    mat_uint32_t array_flags;
    mat_int16_t  array_name_type = MAT_T_INT8;
    int array_flags_type = MAT_T_UINT32, dims_array_type = MAT_T_INT32;
    int array_flags_size = 8;
    int nBytes, i, nzmax = 0;

    mat_uint32_t comp_buf[512];
    mat_uint32_t uncomp_buf[512] = {0,};
    int buf_size = 512;
    size_t byteswritten = 0;

    if ( MAT_C_EMPTY == matvar->class_type ) {
        /* exit early if this is an empty data */
        return byteswritten;
    }

    /* Array Flags */
    array_flags = matvar->class_type & CLASS_TYPE_MASK;
    if ( matvar->isComplex )
        array_flags |= MAT_F_COMPLEX;
    if ( matvar->isGlobal )
        array_flags |= MAT_F_GLOBAL;
    if ( matvar->isLogical )
        array_flags |= MAT_F_LOGICAL;
    if ( matvar->class_type == MAT_C_SPARSE )
        nzmax = ((mat_sparse_t *)matvar->data)->nzmax;
    uncomp_buf[0] = array_flags_type;
    uncomp_buf[1] = array_flags_size;
    uncomp_buf[2] = array_flags;
    uncomp_buf[3] = nzmax;
    /* Rank and Dimension */
    nBytes = matvar->rank * 4;
    uncomp_buf[4] = dims_array_type;
    uncomp_buf[5] = nBytes;
    for ( i = 0; i < matvar->rank; i++ ) {
        mat_int32_t dim;
        dim = matvar->dims[i];
        uncomp_buf[6+i] = dim;
    }
    if ( matvar->rank % 2 != 0 ) {
        int pad4 = 0;
        uncomp_buf[6+i] = pad4;
        i++;
    }

    z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
    z->avail_in = (6+i)*sizeof(*uncomp_buf);
    do {
        z->next_out  = ZLIB_BYTE_PTR(comp_buf);
        z->avail_out = buf_size*sizeof(*comp_buf);
        deflate(z,Z_NO_FLUSH);
        byteswritten += fwrite(comp_buf,1,buf_size*sizeof(*comp_buf)-z->avail_out,
            (FILE*)mat->fp);
    } while ( z->avail_out == 0 );
    /* Name of variable */
    uncomp_buf[0] = array_name_type;
    uncomp_buf[1] = 0;
    z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
    z->avail_in = 8;
    do {
        z->next_out  = ZLIB_BYTE_PTR(comp_buf);
        z->avail_out = buf_size*sizeof(*comp_buf);
        deflate(z,Z_NO_FLUSH);
        byteswritten += fwrite(comp_buf,1,buf_size*sizeof(*comp_buf)-z->avail_out,
            (FILE*)mat->fp);
    } while ( z->avail_out == 0 );

    matvar->internal->datapos = ftell((FILE*)mat->fp);
    if ( matvar->internal->datapos == -1L ) {
        Mat_Critical("Couldn't determine file position");
    }

    byteswritten += WriteCompressedType(mat,matvar,z);
    return byteswritten;
}