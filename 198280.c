WriteCellArrayField(mat_t *mat,matvar_t *matvar)
{
    mat_uint32_t array_flags;
    mat_int16_t array_name_type = MAT_T_INT8;
    int array_flags_type = MAT_T_UINT32, dims_array_type = MAT_T_INT32;
    int array_flags_size = 8, pad4 = 0, matrix_type = MAT_T_MATRIX;
    const mat_int8_t pad1 = 0;
    int nBytes, i, nzmax = 0;
    long start = 0, end = 0;

    if ( matvar == NULL || mat == NULL )
        return 1;

    fwrite(&matrix_type,4,1,(FILE*)mat->fp);
    fwrite(&pad4,4,1,(FILE*)mat->fp);
    if ( MAT_C_EMPTY == matvar->class_type ) {
        /* exit early if this is an empty data */
        return 0;
    }
    start = ftell((FILE*)mat->fp);

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

    if ( mat->byteswap )
        array_flags = Mat_int32Swap((mat_int32_t*)&array_flags);
    fwrite(&array_flags_type,4,1,(FILE*)mat->fp);
    fwrite(&array_flags_size,4,1,(FILE*)mat->fp);
    fwrite(&array_flags,4,1,(FILE*)mat->fp);
    fwrite(&nzmax,4,1,(FILE*)mat->fp);
    /* Rank and Dimension */
    nBytes = matvar->rank * 4;
    fwrite(&dims_array_type,4,1,(FILE*)mat->fp);
    fwrite(&nBytes,4,1,(FILE*)mat->fp);
    for ( i = 0; i < matvar->rank; i++ ) {
        mat_int32_t dim;
        dim = matvar->dims[i];
        fwrite(&dim,4,1,(FILE*)mat->fp);
    }
    if ( matvar->rank % 2 != 0 )
        fwrite(&pad4,4,1,(FILE*)mat->fp);
    /* Name of variable */
    if ( !matvar->name ) {
        fwrite(&array_name_type,2,1,(FILE*)mat->fp);
        fwrite(&pad1,1,1,(FILE*)mat->fp);
        fwrite(&pad1,1,1,(FILE*)mat->fp);
        fwrite(&pad4,4,1,(FILE*)mat->fp);
    } else if ( strlen(matvar->name) <= 4 ) {
        mat_int16_t array_name_len = (mat_int16_t)strlen(matvar->name);
        fwrite(&array_name_type,2,1,(FILE*)mat->fp);
        fwrite(&array_name_len,2,1,(FILE*)mat->fp);
        fwrite(matvar->name,1,array_name_len,(FILE*)mat->fp);
        for ( i = array_name_len; i < 4; i++ )
            fwrite(&pad1,1,1,(FILE*)mat->fp);
    } else {
        mat_int32_t array_name_len = (mat_int32_t)strlen(matvar->name);
        fwrite(&array_name_type,2,1,(FILE*)mat->fp);
        fwrite(&pad1,1,1,(FILE*)mat->fp);
        fwrite(&pad1,1,1,(FILE*)mat->fp);
        fwrite(&array_name_len,4,1,(FILE*)mat->fp);
        fwrite(matvar->name,1,array_name_len,(FILE*)mat->fp);
        if ( array_name_len % 8 )
            for ( i = array_name_len % 8; i < 8; i++ )
                fwrite(&pad1,1,1,(FILE*)mat->fp);
    }

    WriteType(mat,matvar);
    end = ftell((FILE*)mat->fp);
    if ( start != -1L && end != -1L ) {
        nBytes = (int)(end-start);
        (void)fseek((FILE*)mat->fp,(long)-(nBytes+4),SEEK_CUR);
        fwrite(&nBytes,4,1,(FILE*)mat->fp);
        (void)fseek((FILE*)mat->fp,end,SEEK_SET);
    } else {
        Mat_Critical("Couldn't determine file position");
    }

    return 0;
}