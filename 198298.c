GetTypeBufSize(matvar_t *matvar, size_t *size)
{
    int err;
    size_t nBytes, data_bytes;
    size_t tag_size = 8;
    size_t nelems = 1;
    size_t rank_size;

    *size = 0;

    err = SafeMulDims(matvar, &nelems);
    if ( err )
        return 1;

    /* Add rank and dimensions, padded to an 8 byte block */
    err = SafeMul(&rank_size, matvar->rank, 4);
    if ( err )
        return 1;

    if ( matvar->rank % 2 )
        nBytes = tag_size + 4;
    else
        nBytes = tag_size;

    err = SafeAdd(&nBytes, nBytes, rank_size);
    if ( err )
        return 1;

    switch ( matvar->class_type ) {
        case MAT_C_STRUCT:
        {
            matvar_t **fields = (matvar_t**)matvar->data;
            size_t nfields = matvar->internal->num_fields;
            size_t maxlen = 0, i, field_buf_size;

            for ( i = 0; i < nfields; i++ ) {
                char *fieldname = matvar->internal->fieldnames[i];
                if ( NULL != fieldname && strlen(fieldname) > maxlen )
                    maxlen = strlen(fieldname);
            }
            maxlen++;
            while ( nfields*maxlen % 8 != 0 )
                maxlen++;

            err = SafeMul(&field_buf_size, maxlen, nfields);
            err |= SafeAdd(&nBytes, nBytes, tag_size + tag_size);
            err |= SafeAdd(&nBytes, nBytes, field_buf_size);
            if ( err )
                return 1;

            /* FIXME: Add bytes for the fieldnames */
            if ( NULL != fields && nfields > 0 ) {
                size_t nelems_x_nfields = 1;
                err = SafeMul(&nelems_x_nfields, nelems, nfields);
                if ( err )
                    return 1;

                for ( i = 0; i < nelems_x_nfields; i++ ) {
                    err = GetStructFieldBufSize(fields[i], &field_buf_size);
                    err |= SafeAdd(&nBytes, nBytes, tag_size);
                    err |= SafeAdd(&nBytes, nBytes, field_buf_size);
                    if ( err )
                        return 1;
                }
            }
            break;
        }
        case MAT_C_CELL:
        {
            matvar_t **cells = (matvar_t**)matvar->data;

            if ( matvar->nbytes == 0 || matvar->data_size == 0 )
                break;

            nelems = matvar->nbytes / matvar->data_size;
            if ( NULL != cells && nelems > 0 ) {
                size_t i, field_buf_size;
                for ( i = 0; i < nelems; i++ ) {
                    err = GetCellArrayFieldBufSize(cells[i], &field_buf_size);
                    err |= SafeAdd(&nBytes, nBytes, tag_size);
                    err |= SafeAdd(&nBytes, nBytes, field_buf_size);
                    if ( err )
                        return 1;
                }
            }
            break;
        }
        case MAT_C_SPARSE:
        {
            mat_sparse_t *sparse = (mat_sparse_t*)matvar->data;

            err = SafeMul(&data_bytes, sparse->nir, sizeof(mat_int32_t));
            if ( data_bytes % 8 )
                err |= SafeAdd(&data_bytes, data_bytes, 8 - data_bytes % 8);
            err |= SafeAdd(&nBytes, nBytes, tag_size);
            err |= SafeAdd(&nBytes, nBytes, data_bytes);

            err |= SafeMul(&data_bytes, sparse->njc, sizeof(mat_int32_t));
            if ( data_bytes % 8 )
                err |= SafeAdd(&data_bytes, data_bytes, 8 - data_bytes % 8);
            err |= SafeAdd(&nBytes, nBytes, tag_size);
            err |= SafeAdd(&nBytes, nBytes, data_bytes);

            err |= SafeMul(&data_bytes, sparse->ndata, Mat_SizeOf(matvar->data_type));
            if ( data_bytes % 8 )
                err |= SafeAdd(&data_bytes, data_bytes, 8 - data_bytes % 8);
            err |= SafeAdd(&nBytes, nBytes, tag_size);
            err |= SafeAdd(&nBytes, nBytes, data_bytes);

            if ( matvar->isComplex ) {
                err |= SafeAdd(&nBytes, nBytes, tag_size);
                err |= SafeAdd(&nBytes, nBytes, data_bytes);
            }

            if ( err )
                return 1;

            break;
        }
        case MAT_C_CHAR:
            if ( MAT_T_UINT8 == matvar->data_type ||
                 MAT_T_INT8 == matvar->data_type )
                err = SafeMul(&data_bytes, nelems, Mat_SizeOf(MAT_T_UINT16));
            else
                err = SafeMul(&data_bytes, nelems, Mat_SizeOf(matvar->data_type));
            if ( data_bytes % 8 )
                err |= SafeAdd(&data_bytes, data_bytes, 8 - data_bytes % 8);

            err |= SafeAdd(&nBytes, nBytes, tag_size);
            err |= SafeAdd(&nBytes, nBytes, data_bytes);

            if ( matvar->isComplex ) {
                err |= SafeAdd(&nBytes, nBytes, tag_size);
                err |= SafeAdd(&nBytes, nBytes, data_bytes);
            }

            if ( err )
                return 1;

            break;
        default:
            err = SafeMul(&data_bytes, nelems, Mat_SizeOf(matvar->data_type));
            if ( data_bytes % 8 )
                err |= SafeAdd(&data_bytes, data_bytes, 8 - data_bytes % 8);

            err |= SafeAdd(&nBytes, nBytes, tag_size);
            err |= SafeAdd(&nBytes, nBytes, data_bytes);

            if ( matvar->isComplex ) {
                err |= SafeAdd(&nBytes, nBytes, tag_size);
                err |= SafeAdd(&nBytes, nBytes, data_bytes);
            }

            if ( err )
                return 1;
    } /* switch ( matvar->class_type ) */

    *size = nBytes;
    return 0;
}