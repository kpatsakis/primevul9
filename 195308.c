Mat_VarGetSize(matvar_t *matvar)
{
    int err;
    size_t i;
    size_t bytes = 0, overhead = 0, ptr = 0;

#if defined(_WIN64) || (defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 8)) || (defined(SIZEOF_VOID_P) && (SIZEOF_VOID_P == 8))
    /* 112 bytes cell/struct overhead for 64-bit system */
    overhead = 112;
    ptr = 8;
#elif defined(_WIN32) || (defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 4)) || (defined(SIZEOF_VOID_P) && (SIZEOF_VOID_P == 4))
    /* 60 bytes cell/struct overhead for 32-bit system */
    overhead = 60;
    ptr = 4;
#endif

    if ( matvar->class_type == MAT_C_STRUCT ) {
        matvar_t **fields = (matvar_t**)matvar->data;
        size_t field_name_length;
        if ( NULL != fields ) {
            size_t nelems_x_nfields = matvar->internal->num_fields;
            err = SafeMulDims(matvar, &nelems_x_nfields);
            err |= SafeMul(&bytes, nelems_x_nfields, overhead);
            if ( err )
                return 0;

            for ( i = 0; i < nelems_x_nfields; i++ ) {
                if ( NULL != fields[i] ) {
                    if ( MAT_C_EMPTY != fields[i]->class_type ) {
                        err = SafeAdd(&bytes, bytes, Mat_VarGetSize(fields[i]));
                        if ( err )
                            return 0;
                    } else {
                        bytes -= overhead;
                        bytes += ptr;
                    }
                }
            }
        }
        err = SafeMul(&field_name_length, 64 /* max field name length */, matvar->internal->num_fields);
        err |= SafeAdd(&bytes, bytes, field_name_length);
        if ( err )
            return 0;
    } else if ( matvar->class_type == MAT_C_CELL ) {
        matvar_t **cells = (matvar_t**)matvar->data;
        if ( NULL != cells ) {
            size_t nelems = matvar->nbytes / matvar->data_size;
            err = SafeMul(&bytes, nelems, overhead);
            if ( err )
                return 0;

            for ( i = 0; i < nelems; i++ ) {
                if ( NULL != cells[i] ) {
                    if ( MAT_C_EMPTY != cells[i]->class_type ) {
                        err = SafeAdd(&bytes, bytes, Mat_VarGetSize(cells[i]));
                        if ( err )
                            return 0;
                    } else {
                        bytes -= overhead;
                        bytes += ptr;
                    }
                }
            }
        }
    } else if ( matvar->class_type == MAT_C_SPARSE ) {
        mat_sparse_t *sparse = (mat_sparse_t*)matvar->data;
        if ( NULL != sparse ) {
            size_t sparse_size = 0;
            err = SafeMul(&bytes, sparse->ndata, Mat_SizeOf(matvar->data_type));
            if ( err )
                return 0;

            if ( matvar->isComplex ) {
                err = SafeMul(&bytes, bytes, 2);
                if ( err )
                    return 0;
            }

#if defined(_WIN64) || (defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 8)) || (defined(SIZEOF_VOID_P) && (SIZEOF_VOID_P == 8))
            /* 8 byte integers for 64-bit system (as displayed in MATLAB (x64) whos) */
            err = SafeMul(&sparse_size, sparse->nir + sparse->njc, 8);
#elif defined(_WIN32) || (defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 4)) || (defined(SIZEOF_VOID_P) && (SIZEOF_VOID_P == 4))
            /* 4 byte integers for 32-bit system (as defined by mat_sparse_t) */
            err = SafeMul(&sparse_size, sparse->nir + sparse->njc, 4);
#endif
            err |= SafeAdd(&bytes, bytes, sparse_size);
            if ( err )
                return 0;

            if ( sparse->ndata == 0 || sparse->nir == 0 || sparse->njc == 0 ) {
                err = SafeAdd(&bytes, bytes, matvar->isLogical ? 1 : 8);
                if ( err )
                    return 0;
            }
        }
    } else {
        if ( matvar->rank > 0 ) {
            bytes = Mat_SizeOfClass(matvar->class_type);
            err = SafeMulDims(matvar, &bytes);
            if ( err )
                return 0;

            if ( matvar->isComplex ) {
                err = SafeMul(&bytes, bytes, 2);
                if (err)
                    return 0;
            }
        }
    }

    return bytes;
}