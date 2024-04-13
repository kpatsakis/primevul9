WriteType(mat_t *mat,matvar_t *matvar)
{
    int err;
    mat_int16_t array_name_type = MAT_T_INT8;
    mat_int8_t pad1 = 0;
    int nBytes, j;
    size_t nelems = 1;

    err = SafeMulDims(matvar, &nelems);
    if ( err )
        return err;

    switch ( matvar->class_type ) {
        case MAT_C_DOUBLE:
        case MAT_C_SINGLE:
        case MAT_C_INT64:
        case MAT_C_UINT64:
        case MAT_C_INT32:
        case MAT_C_UINT32:
        case MAT_C_INT16:
        case MAT_C_UINT16:
        case MAT_C_INT8:
        case MAT_C_UINT8:
        {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)matvar->data;

                if ( NULL == matvar->data )
                    complex_data = &null_complex_data;

                nBytes=WriteData(mat,complex_data->Re,nelems,matvar->data_type);
                if ( nBytes % 8 )
                    for ( j = nBytes % 8; j < 8; j++ )
                        fwrite(&pad1,1,1,(FILE*)mat->fp);
                nBytes=WriteData(mat,complex_data->Im,nelems,matvar->data_type);
                if ( nBytes % 8 )
                    for ( j = nBytes % 8; j < 8; j++ )
                        fwrite(&pad1,1,1,(FILE*)mat->fp);
            } else {
                nBytes=WriteData(mat,matvar->data,nelems,matvar->data_type);
                if ( nBytes % 8 )
                    for ( j = nBytes % 8; j < 8; j++ )
                        fwrite(&pad1,1,1,(FILE*)mat->fp);
            }
            break;
        }
        case MAT_C_CHAR:
            nBytes=WriteCharData(mat,matvar->data,nelems,matvar->data_type);
            break;
        case MAT_C_CELL:
        {
            size_t i;
            matvar_t **cells = (matvar_t **)matvar->data;

            /* Check for an empty cell array */
            if ( matvar->nbytes == 0 || matvar->data_size == 0 ||
                 matvar->data   == NULL )
                break;
            nelems = matvar->nbytes / matvar->data_size;
            for ( i = 0; i < nelems; i++ )
                WriteCellArrayField(mat,cells[i]);
            break;
        }
        case MAT_C_STRUCT:
        {
            mat_int16_t fieldname_type = MAT_T_INT32;
            mat_int16_t fieldname_data_size = 4;
            char *padzero;
            int fieldname_size;
            size_t maxlen = 0, nfields, i, nelems_x_nfields;
            matvar_t **fields = (matvar_t **)matvar->data;
            unsigned fieldname;

            /* nelems*matvar->data_size can be zero when saving a struct that
             * contains an empty struct in one of its fields
             * (e.g. x.y = struct('z', {})). If it's zero, we would divide
             * by zero.
             */
            nfields = matvar->internal->num_fields;
            /* Check for a structure with no fields */
            if ( nfields < 1 ) {
#if 0
                fwrite(&fieldname_type,2,1,(FILE*)mat->fp);
                fwrite(&fieldname_data_size,2,1,(FILE*)mat->fp);
#else
                fieldname = (fieldname_data_size<<16) | fieldname_type;
                fwrite(&fieldname,4,1,(FILE*)mat->fp);
#endif
                fieldname_size = 1;
                fwrite(&fieldname_size,4,1,(FILE*)mat->fp);
                fwrite(&array_name_type,2,1,(FILE*)mat->fp);
                fwrite(&pad1,1,1,(FILE*)mat->fp);
                fwrite(&pad1,1,1,(FILE*)mat->fp);
                nBytes = 0;
                fwrite(&nBytes,4,1,(FILE*)mat->fp);
                break;
            }

            for ( i = 0; i < nfields; i++ ) {
                size_t len = strlen(matvar->internal->fieldnames[i]);
                if ( len > maxlen )
                    maxlen = len;
            }
            maxlen++;
            fieldname_size = maxlen;
            while ( nfields*fieldname_size % 8 != 0 )
                fieldname_size++;
#if 0
            fwrite(&fieldname_type,2,1,(FILE*)mat->fp);
            fwrite(&fieldname_data_size,2,1,(FILE*)mat->fp);
#else
            fieldname = (fieldname_data_size<<16) | fieldname_type;
            fwrite(&fieldname,4,1,(FILE*)mat->fp);
#endif
            fwrite(&fieldname_size,4,1,(FILE*)mat->fp);
            fwrite(&array_name_type,2,1,(FILE*)mat->fp);
            fwrite(&pad1,1,1,(FILE*)mat->fp);
            fwrite(&pad1,1,1,(FILE*)mat->fp);
            nBytes = nfields*fieldname_size;
            fwrite(&nBytes,4,1,(FILE*)mat->fp);
            padzero = (char*)calloc(fieldname_size,1);
            for ( i = 0; i < nfields; i++ ) {
                size_t len = strlen(matvar->internal->fieldnames[i]);
                fwrite(matvar->internal->fieldnames[i],1,len,(FILE*)mat->fp);
                fwrite(padzero,1,fieldname_size-len,(FILE*)mat->fp);
            }
            free(padzero);
            err = SafeMul(&nelems_x_nfields, nelems, nfields);
            if ( err )
                break;
            for ( i = 0; i < nelems_x_nfields; i++ )
                WriteStructField(mat,fields[i]);
            break;
        }
        case MAT_C_SPARSE:
        {
            mat_sparse_t *sparse = (mat_sparse_t*)matvar->data;

            nBytes = WriteData(mat,sparse->ir,sparse->nir,MAT_T_INT32);
            if ( nBytes % 8 )
                for ( j = nBytes % 8; j < 8; j++ )
                    fwrite(&pad1,1,1,(FILE*)mat->fp);
            nBytes = WriteData(mat,sparse->jc,sparse->njc,MAT_T_INT32);
            if ( nBytes % 8 )
                for ( j = nBytes % 8; j < 8; j++ )
                    fwrite(&pad1,1,1,(FILE*)mat->fp);
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)sparse->data;
                nBytes = WriteData(mat,complex_data->Re,sparse->ndata,
                                   matvar->data_type);
                if ( nBytes % 8 )
                    for ( j = nBytes % 8; j < 8; j++ )
                        fwrite(&pad1,1,1,(FILE*)mat->fp);
                nBytes = WriteData(mat,complex_data->Im,sparse->ndata,
                                   matvar->data_type);
                if ( nBytes % 8 )
                    for ( j = nBytes % 8; j < 8; j++ )
                        fwrite(&pad1,1,1,(FILE*)mat->fp);
            } else {
                nBytes = WriteData(mat,sparse->data,sparse->ndata,
                                   matvar->data_type);
                if ( nBytes % 8 )
                    for ( j = nBytes % 8; j < 8; j++ )
                        fwrite(&pad1,1,1,(FILE*)mat->fp);
            }
        }
        case MAT_C_FUNCTION:
        case MAT_C_OBJECT:
        case MAT_C_EMPTY:
        case MAT_C_OPAQUE:
            break;
    }

    return err;
}