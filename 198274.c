WriteCompressedType(mat_t *mat,matvar_t *matvar,z_streamp z)
{
    int err;
    mat_uint32_t comp_buf[512];
    mat_uint32_t uncomp_buf[512] = {0,};
    size_t byteswritten = 0, nelems = 1;

    if ( MAT_C_EMPTY == matvar->class_type ) {
        /* exit early if this is an empty data */
        return byteswritten;
    }

    err = SafeMulDims(matvar, &nelems);
    if ( err ) {
        Mat_Critical("Integer multiplication overflow");
        return byteswritten;
    }

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
            /* WriteCompressedData makes sure uncompressed data is aligned
             * on an 8-byte boundary */
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)matvar->data;

                if ( NULL == matvar->data )
                    complex_data = &null_complex_data;

                byteswritten += WriteCompressedData(mat,z,
                    complex_data->Re,nelems,matvar->data_type);
                byteswritten += WriteCompressedData(mat,z,
                    complex_data->Im,nelems,matvar->data_type);
            } else {
                byteswritten += WriteCompressedData(mat,z,
                    matvar->data,nelems,matvar->data_type);
            }
            break;
        }
        case MAT_C_CHAR:
        {
            byteswritten += WriteCompressedCharData(mat,z,matvar->data,
                nelems,matvar->data_type);
            break;
        }
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
                WriteCompressedCellArrayField(mat,cells[i],z);
            break;
        }
        case MAT_C_STRUCT:
        {
            int buf_size = 512;
            mat_int16_t fieldname_type = MAT_T_INT32;
            mat_int16_t fieldname_data_size = 4;
            unsigned char *padzero;
            int fieldname_size;
            size_t maxlen = 0, nfields, i, nelems_x_nfields;
            mat_int32_t array_name_type = MAT_T_INT8;
            matvar_t **fields = (matvar_t **)matvar->data;

            nfields = matvar->internal->num_fields;
            /* Check for a structure with no fields */
            if ( nfields < 1 ) {
                fieldname_size = 1;
                uncomp_buf[0] = (fieldname_data_size << 16) | fieldname_type;
                uncomp_buf[1] = fieldname_size;
                uncomp_buf[2] = array_name_type;
                uncomp_buf[3] = 0;
                z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
                z->avail_in = 16;
                do {
                    z->next_out  = ZLIB_BYTE_PTR(comp_buf);
                    z->avail_out = buf_size*sizeof(*comp_buf);
                    deflate(z,Z_NO_FLUSH);
                    byteswritten += fwrite(comp_buf,1,buf_size*
                        sizeof(*comp_buf)-z->avail_out,(FILE*)mat->fp);
                } while ( z->avail_out == 0 );
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
            uncomp_buf[0] = (fieldname_data_size << 16) | fieldname_type;
            uncomp_buf[1] = fieldname_size;
            uncomp_buf[2] = array_name_type;
            uncomp_buf[3] = nfields*fieldname_size;

            padzero = (unsigned char*)calloc(fieldname_size,1);
            z->next_in  = ZLIB_BYTE_PTR(uncomp_buf);
            z->avail_in = 16;
            do {
                z->next_out  = ZLIB_BYTE_PTR(comp_buf);
                z->avail_out = buf_size*sizeof(*comp_buf);
                deflate(z,Z_NO_FLUSH);
                byteswritten += fwrite(comp_buf,1,
                    buf_size*sizeof(*comp_buf)-z->avail_out,(FILE*)mat->fp);
            } while ( z->avail_out == 0 );
            for ( i = 0; i < nfields; i++ ) {
                size_t len = strlen(matvar->internal->fieldnames[i]);
                memset(padzero,'\0',fieldname_size);
                memcpy(padzero,matvar->internal->fieldnames[i],len);
                z->next_in  = ZLIB_BYTE_PTR(padzero);
                z->avail_in = fieldname_size;
                do {
                    z->next_out  = ZLIB_BYTE_PTR(comp_buf);
                    z->avail_out = buf_size*sizeof(*comp_buf);
                    deflate(z,Z_NO_FLUSH);
                    byteswritten += fwrite(comp_buf,1,
                        buf_size*sizeof(*comp_buf)-z->avail_out,(FILE*)mat->fp);
                } while ( z->avail_out == 0 );
            }
            free(padzero);
            err = SafeMul(&nelems_x_nfields, nelems, nfields);
            if ( err ) {
                Mat_Critical("Integer multiplication overflow");
                return byteswritten;
            }
            for ( i = 0; i < nelems_x_nfields; i++ )
                byteswritten += WriteCompressedStructField(mat,fields[i],z);
            break;
        }
        case MAT_C_SPARSE:
        {
            mat_sparse_t *sparse = (mat_sparse_t*)matvar->data;

            byteswritten += WriteCompressedData(mat,z,sparse->ir,
                sparse->nir,MAT_T_INT32);
            byteswritten += WriteCompressedData(mat,z,sparse->jc,
                sparse->njc,MAT_T_INT32);
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)sparse->data;
                byteswritten += WriteCompressedData(mat,z,
                    complex_data->Re,sparse->ndata,matvar->data_type);
                byteswritten += WriteCompressedData(mat,z,
                    complex_data->Im,sparse->ndata,matvar->data_type);
            } else {
                byteswritten += WriteCompressedData(mat,z,
                    sparse->data,sparse->ndata,matvar->data_type);
            }
            break;
        }
        case MAT_C_FUNCTION:
        case MAT_C_OBJECT:
        case MAT_C_EMPTY:
        case MAT_C_OPAQUE:
            break;
    }

    return byteswritten;
}