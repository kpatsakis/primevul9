ReadRankDims(mat_t *mat, matvar_t *matvar, enum matio_types data_type, mat_uint32_t nbytes)
{
    size_t bytesread = 0;
    /* Rank and dimension */
    if ( data_type == MAT_T_INT32 ) {
        matvar->rank = nbytes / sizeof(mat_uint32_t);
        matvar->dims = (size_t*)malloc(matvar->rank*sizeof(*matvar->dims));
        if ( NULL != matvar->dims ) {
            int i;
            mat_uint32_t buf;

            for ( i = 0; i < matvar->rank; i++) {
                size_t readresult = fread(&buf, sizeof(mat_uint32_t), 1, (FILE*)mat->fp);
                if ( 1 == readresult ) {
                    bytesread += sizeof(mat_uint32_t);
                    if ( mat->byteswap ) {
                        matvar->dims[i] = Mat_uint32Swap(&buf);
                    } else {
                        matvar->dims[i] = buf;
                    }
                } else {
                    free(matvar->dims);
                    matvar->dims = NULL;
                    matvar->rank = 0;
                    return 0;
                }
            }

            if ( matvar->rank % 2 != 0 ) {
                size_t readresult = fread(&buf, sizeof(mat_uint32_t), 1, (FILE*)mat->fp);
                if ( 1 == readresult ) {
                    bytesread += sizeof(mat_uint32_t);
                } else {
                    free(matvar->dims);
                    matvar->dims = NULL;
                    matvar->rank = 0;
                    return 0;
                }
            }
        } else {
            matvar->rank = 0;
            Mat_Critical("Error allocating memory for dims");
        }
    }
    return bytesread;
}