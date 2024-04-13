Mat_VarRead5(mat_t *mat, matvar_t *matvar)
{
    int nBytes = 0, byteswap, data_in_tag = 0, err;
    size_t nelems = 1;
    enum matio_types packed_type = MAT_T_UNKNOWN;
    long fpos;
    mat_uint32_t tag[2];
    size_t bytesread = 0;

    if ( matvar == NULL )
        return;
    else if ( matvar->rank == 0 )        /* An empty data set */
        return;
#if defined(HAVE_ZLIB)
    else if ( NULL != matvar->internal->data ) {
        /* Data already read in ReadNextStructField or ReadNextCell */
        matvar->data = matvar->internal->data;
        matvar->internal->data = NULL;
        return;
    }
#endif
    fpos = ftell((FILE*)mat->fp);
    if ( fpos == -1L ) {
        Mat_Critical("Couldn't determine file position");
        return;
    }
    err = SafeMulDims(matvar, &nelems);
    if ( err ) {
        Mat_Critical("Integer multiplication overflow");
        return;
    }
    byteswap = mat->byteswap;
    switch ( matvar->class_type ) {
        case MAT_C_EMPTY:
            matvar->nbytes = 0;
            matvar->data_size = sizeof(double);
            matvar->data_type = MAT_T_DOUBLE;
            matvar->rank = 2;
            matvar->dims = (size_t*)malloc(matvar->rank*sizeof(*(matvar->dims)));
            matvar->dims[0] = 0;
            matvar->dims[1] = 0;
            break;
        case MAT_C_DOUBLE:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(double);
            matvar->data_type = MAT_T_DOUBLE;
            break;
        case MAT_C_SINGLE:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(float);
            matvar->data_type = MAT_T_SINGLE;
            break;
        case MAT_C_INT64:
#ifdef HAVE_MAT_INT64_T
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_int64_t);
            matvar->data_type = MAT_T_INT64;
#endif
            break;
        case MAT_C_UINT64:
#ifdef HAVE_MAT_UINT64_T
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_uint64_t);
            matvar->data_type = MAT_T_UINT64;
#endif
            break;
        case MAT_C_INT32:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_int32_t);
            matvar->data_type = MAT_T_INT32;
            break;
        case MAT_C_UINT32:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_uint32_t);
            matvar->data_type = MAT_T_UINT32;
            break;
        case MAT_C_INT16:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_int16_t);
            matvar->data_type = MAT_T_INT16;
            break;
        case MAT_C_UINT16:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_uint16_t);
            matvar->data_type = MAT_T_UINT16;
            break;
        case MAT_C_INT8:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_int8_t);
            matvar->data_type = MAT_T_INT8;
            break;
        case MAT_C_UINT8:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            matvar->data_size = sizeof(mat_uint8_t);
            matvar->data_type = MAT_T_UINT8;
            break;
        case MAT_C_CHAR:
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if defined(HAVE_ZLIB)
                matvar->internal->z->avail_in = 0;
                InflateDataType(mat,matvar->internal->z,tag);
                if ( byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    nBytes = (tag[0] & 0xffff0000) >> 16;
                } else {
                    data_in_tag = 0;
                    InflateDataType(mat,matvar->internal->z,tag+1);
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag+1);
                    nBytes = tag[1];
                }
#endif
                matvar->data_type = packed_type;
                matvar->data_size = Mat_SizeOf(matvar->data_type);
                matvar->nbytes = nBytes;
            } else {
                bytesread += fread(tag,4,1,(FILE*)mat->fp);
                if ( byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    /* nBytes = (tag[0] & 0xffff0000) >> 16; */
                } else {
                    data_in_tag = 0;
                    bytesread += fread(tag+1,4,1,(FILE*)mat->fp);
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag+1);
                    /* nBytes = tag[1]; */
                }
                matvar->data_type = MAT_T_UINT8;
                matvar->data_size = Mat_SizeOf(MAT_T_UINT8);
                err = SafeMul(&matvar->nbytes, nelems, matvar->data_size);
                if ( err ) {
                    Mat_Critical("Integer multiplication overflow");
                    break;
                }
            }
            if ( matvar->isComplex ) {
                break;
            }
            matvar->data = calloc(matvar->nbytes+1,1);
            if ( NULL == matvar->data ) {
                Mat_Critical("Couldn't allocate memory for the data");
                break;
            }
            if ( 0 == matvar->nbytes ) {
                break;
            }
            {
                size_t nbytes;
                err = SafeMul(&nbytes, nelems, matvar->data_size);
                if ( err || nbytes > matvar->nbytes ) {
                    break;
                }
            }
            if ( matvar->compression == MAT_COMPRESSION_NONE ) {
                nBytes = ReadCharData(mat,(char*)matvar->data,packed_type,(int)nelems);
                /*
                 * If the data was in the tag we started on a 4-byte
                 * boundary so add 4 to make it an 8-byte
                 */
                if ( data_in_tag )
                    nBytes+=4;
                if ( (nBytes % 8) != 0 )
                    (void)fseek((FILE*)mat->fp,8-(nBytes % 8),SEEK_CUR);
#if defined(HAVE_ZLIB)
            } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
                nBytes = ReadCompressedCharData(mat,matvar->internal->z,
                             (char*)matvar->data,packed_type,(int)nelems);
                /*
                 * If the data was in the tag we started on a 4-byte
                 * boundary so add 4 to make it an 8-byte
                 */
                if ( data_in_tag )
                    nBytes+=4;
                if ( (nBytes % 8) != 0 )
                    InflateSkip(mat,matvar->internal->z,8-(nBytes % 8));
#endif
            }
            break;
        case MAT_C_STRUCT:
        {
            matvar_t **fields;
            size_t i, nelems_x_nfields;

            matvar->data_type = MAT_T_STRUCT;
            err = SafeMul(&nelems_x_nfields, nelems, matvar->internal->num_fields);
            if ( err || !matvar->nbytes || !matvar->data_size || NULL == matvar->data )
                break;
            fields = (matvar_t **)matvar->data;
            for ( i = 0; i < nelems_x_nfields; i++ ) {
                if ( NULL != fields[i] ) {
                    Mat_VarRead5(mat,fields[i]);
                }
            }
            break;
        }
        case MAT_C_CELL:
        {
            matvar_t **cells;
            size_t i;

            if ( NULL == matvar->data ) {
                Mat_Critical("Data is NULL for cell array %s",matvar->name);
                break;
            }
            cells = (matvar_t **)matvar->data;
            for ( i = 0; i < nelems; i++ ) {
                if ( NULL != cells[i] ) {
                    Mat_VarRead5(mat, cells[i]);
                }
            }
            /* FIXME: */
            matvar->data_type = MAT_T_CELL;
            break;
        }
        case MAT_C_SPARSE:
        {
            mat_int32_t N = 0;
            mat_sparse_t *data;

            matvar->data_size = sizeof(mat_sparse_t);
            matvar->data      = malloc(matvar->data_size);
            if ( matvar->data == NULL ) {
                Mat_Critical("Mat_VarRead5: Allocation of data pointer failed");
                break;
            }
            data = (mat_sparse_t*)matvar->data;
            data->nzmax  = matvar->nbytes;
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
            /*  Read ir    */
            bytesread += ReadSparse(mat, matvar, &data->nir, &data->ir);
            /*  Read jc    */
            bytesread += ReadSparse(mat, matvar, &data->njc, &data->jc);
            /*  Read data  */
            if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if defined(HAVE_ZLIB)
                matvar->internal->z->avail_in = 0;
                InflateDataType(mat,matvar->internal->z,tag);
                if ( mat->byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    N = (tag[0] & 0xffff0000) >> 16;
                } else {
                    data_in_tag = 0;
                    (void)ReadCompressedInt32Data(mat,matvar->internal->z,
                             (mat_int32_t*)&N,MAT_T_INT32,1);
                }
#endif
            } else {
                bytesread += fread(tag,4,1,(FILE*)mat->fp);
                if ( mat->byteswap )
                    (void)Mat_uint32Swap(tag);
                packed_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                    data_in_tag = 1;
                    N = (tag[0] & 0xffff0000) >> 16;
                } else {
                    data_in_tag = 0;
                    bytesread += fread(&N,4,1,(FILE*)mat->fp);
                    if ( mat->byteswap )
                        (void)Mat_int32Swap(&N);
                }
            }
            if ( matvar->isLogical && packed_type == MAT_T_DOUBLE ) {
                /* For some reason, MAT says the data type is a double,
                 * but it appears to be written as 8-bit unsigned integer.
                 */
                packed_type = MAT_T_UINT8;
            }
#if defined(EXTENDED_SPARSE)
            matvar->data_type = packed_type;
#else
            matvar->data_type = MAT_T_DOUBLE;
#endif
            {
                size_t s_type = Mat_SizeOf(packed_type);
                if ( s_type == 0 )
                    break;
                data->ndata = N / s_type;
            }
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data =
                    ComplexMalloc(data->ndata*Mat_SizeOf(matvar->data_type));
                if ( NULL == complex_data ) {
                    Mat_Critical("Couldn't allocate memory for the complex sparse data");
                    break;
                }
                if ( matvar->compression == MAT_COMPRESSION_NONE ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadDoubleData(mat,(double*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadSingleData(mat,(float*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadInt64Data(mat,(mat_int64_t*)complex_data->Re,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadUInt64Data(mat,(mat_uint64_t*)complex_data->Re,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadInt32Data(mat,(mat_int32_t*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadUInt32Data(mat,(mat_uint32_t*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadInt16Data(mat,(mat_int16_t*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadUInt16Data(mat,(mat_uint16_t*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadInt8Data(mat,(mat_int8_t*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadUInt8Data(mat,(mat_uint8_t*)complex_data->Re,
                                packed_type,data->ndata);
                            break;
                        default:
                            break;
                    }
#else
                    nBytes = ReadDoubleData(mat,(double*)complex_data->Re,
                                 packed_type,data->ndata);
#endif
                    if ( data_in_tag )
                        nBytes+=4;
                    if ( (nBytes % 8) != 0 )
                        (void)fseek((FILE*)mat->fp,8-(nBytes % 8),SEEK_CUR);

                    /* Complex Data Tag */
                    bytesread += fread(tag,4,1,(FILE*)mat->fp);
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag);
                    packed_type = TYPE_FROM_TAG(tag[0]);
                    if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                        data_in_tag = 1;
                        nBytes = (tag[0] & 0xffff0000) >> 16;
                    } else {
                        data_in_tag = 0;
                        bytesread += fread(tag+1,4,1,(FILE*)mat->fp);
                        if ( byteswap )
                            (void)Mat_uint32Swap(tag+1);
                        nBytes = tag[1];
                    }
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadDoubleData(mat,(double*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadSingleData(mat,(float*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadInt64Data(mat,(mat_int64_t*)complex_data->Im,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadUInt64Data(mat,(mat_uint64_t*)complex_data->Im,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadInt32Data(mat,(mat_int32_t*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadUInt32Data(mat,(mat_uint32_t*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadInt16Data(mat,(mat_int16_t*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadUInt16Data(mat,(mat_uint16_t*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadInt8Data(mat,(mat_int8_t*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadUInt8Data(mat,(mat_uint8_t*)complex_data->Im,
                                packed_type,data->ndata);
                            break;
                        default:
                            break;
                    }
#else /* EXTENDED_SPARSE */
                    nBytes = ReadDoubleData(mat,(double*)complex_data->Im,
                                packed_type,data->ndata);
#endif /* EXTENDED_SPARSE */
                    if ( data_in_tag )
                        nBytes+=4;
                    if ( (nBytes % 8) != 0 )
                        (void)fseek((FILE*)mat->fp,8-(nBytes % 8),SEEK_CUR);
#if defined(HAVE_ZLIB)
                } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadCompressedDoubleData(mat,matvar->internal->z,
                                 (double*)complex_data->Re,packed_type,data->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadCompressedSingleData(mat,matvar->internal->z,
                                 (float*)complex_data->Re,packed_type,data->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadCompressedInt64Data(mat,
                                matvar->internal->z,(mat_int64_t*)complex_data->Re,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadCompressedUInt64Data(mat,
                                matvar->internal->z,(mat_uint64_t*)complex_data->Re,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadCompressedInt32Data(mat,matvar->internal->z,
                                 (mat_int32_t*)complex_data->Re,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadCompressedUInt32Data(mat,matvar->internal->z,
                                 (mat_uint32_t*)complex_data->Re,packed_type,data->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadCompressedInt16Data(mat,matvar->internal->z,
                                 (mat_int16_t*)complex_data->Re,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadCompressedUInt16Data(mat,matvar->internal->z,
                                 (mat_uint16_t*)complex_data->Re,packed_type,data->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadCompressedInt8Data(mat,matvar->internal->z,
                                 (mat_int8_t*)complex_data->Re,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadCompressedUInt8Data(mat,matvar->internal->z,
                                 (mat_uint8_t*)complex_data->Re,packed_type,data->ndata);
                            break;
                        default:
                            break;
                    }
#else    /* EXTENDED_SPARSE */
                    nBytes = ReadCompressedDoubleData(mat,matvar->internal->z,
                                 (double*)complex_data->Re,packed_type,data->ndata);
#endif    /* EXTENDED_SPARSE */
                    if ( data_in_tag )
                        nBytes+=4;
                    if ( (nBytes % 8) != 0 )
                        InflateSkip(mat,matvar->internal->z,8-(nBytes % 8));

                    /* Complex Data Tag */
                    InflateDataType(mat,matvar->internal->z,tag);
                    if ( byteswap )
                        (void)Mat_uint32Swap(tag);

                    packed_type = TYPE_FROM_TAG(tag[0]);
                    if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
                        data_in_tag = 1;
                        nBytes = (tag[0] & 0xffff0000) >> 16;
                    } else {
                        data_in_tag = 0;
                        InflateDataType(mat,matvar->internal->z,tag+1);
                        if ( byteswap )
                            (void)Mat_uint32Swap(tag+1);
                        nBytes = tag[1];
                    }
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadCompressedDoubleData(mat,matvar->internal->z,
                                 (double*)complex_data->Im,packed_type,data->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadCompressedSingleData(mat,matvar->internal->z,
                                 (float*)complex_data->Im,packed_type,data->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadCompressedInt64Data(mat,
                                matvar->internal->z,(mat_int64_t*)complex_data->Im,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadCompressedUInt64Data(mat,
                                matvar->internal->z,(mat_uint64_t*)complex_data->Im,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadCompressedInt32Data(mat,matvar->internal->z,
                                 (mat_int32_t*)complex_data->Im,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadCompressedUInt32Data(mat,matvar->internal->z,
                                 (mat_uint32_t*)complex_data->Im,packed_type,data->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadCompressedInt16Data(mat,matvar->internal->z,
                                 (mat_int16_t*)complex_data->Im,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadCompressedUInt16Data(mat,matvar->internal->z,
                                 (mat_uint16_t*)complex_data->Im,packed_type,data->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadCompressedInt8Data(mat,matvar->internal->z,
                                 (mat_int8_t*)complex_data->Im,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadCompressedUInt8Data(mat,matvar->internal->z,
                                 (mat_uint8_t*)complex_data->Im,packed_type,data->ndata);
                            break;
                        default:
                            break;
                    }
#else    /* EXTENDED_SPARSE */
                    nBytes = ReadCompressedDoubleData(mat,matvar->internal->z,
                                 (double*)complex_data->Im,packed_type,data->ndata);
#endif    /* EXTENDED_SPARSE */
                    if ( data_in_tag )
                        nBytes+=4;
                    if ( (nBytes % 8) != 0 )
                        InflateSkip(mat,matvar->internal->z,8-(nBytes % 8));
#endif    /* HAVE_ZLIB */
                }
                data->data = complex_data;
            } else { /* isComplex */
                data->data = malloc(data->ndata*Mat_SizeOf(matvar->data_type));
                if ( data->data == NULL ) {
                    Mat_Critical("Couldn't allocate memory for the sparse data");
                    break;
                }
                if ( matvar->compression == MAT_COMPRESSION_NONE ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadDoubleData(mat,(double*)data->data,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadSingleData(mat,(float*)data->data,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadInt64Data(mat,(mat_int64_t*)data->data,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadUInt64Data(mat,(mat_uint64_t*)data->data,
                                packed_type,data->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadInt32Data(mat,(mat_int32_t*)data->data,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadUInt32Data(mat,(mat_uint32_t*)data->data,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadInt16Data(mat,(mat_int16_t*)data->data,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadUInt16Data(mat,(mat_uint16_t*)data->data,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadInt8Data(mat,(mat_int8_t*)data->data,
                                packed_type,data->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadUInt8Data(mat,(mat_uint8_t*)data->data,
                                packed_type,data->ndata);
                            break;
                        default:
                            break;
                    }
#else
                    nBytes = ReadDoubleData(mat,(double*)data->data,packed_type,
                                 data->ndata);
#endif
                    if ( data_in_tag )
                        nBytes+=4;
                    if ( (nBytes % 8) != 0 )
                        (void)fseek((FILE*)mat->fp,8-(nBytes % 8),SEEK_CUR);
#if defined(HAVE_ZLIB)
                } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if defined(EXTENDED_SPARSE)
                    switch ( matvar->data_type ) {
                        case MAT_T_DOUBLE:
                            nBytes = ReadCompressedDoubleData(mat,matvar->internal->z,
                                 (double*)data->data,packed_type,data->ndata);
                            break;
                        case MAT_T_SINGLE:
                            nBytes = ReadCompressedSingleData(mat,matvar->internal->z,
                                 (float*)data->data,packed_type,data->ndata);
                            break;
                        case MAT_T_INT64:
#ifdef HAVE_MAT_INT64_T
                            nBytes = ReadCompressedInt64Data(mat,
                                matvar->internal->z,(mat_int64_t*)data->data,packed_type,
                                data->ndata);
#endif
                            break;
                        case MAT_T_UINT64:
#ifdef HAVE_MAT_UINT64_T
                            nBytes = ReadCompressedUInt64Data(mat,
                                matvar->internal->z,(mat_uint64_t*)data->data,packed_type,
                                data->ndata);
#endif
                            break;
                        case MAT_T_INT32:
                            nBytes = ReadCompressedInt32Data(mat,matvar->internal->z,
                                 (mat_int32_t*)data->data,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT32:
                            nBytes = ReadCompressedUInt32Data(mat,matvar->internal->z,
                                 (mat_uint32_t*)data->data,packed_type,data->ndata);
                            break;
                        case MAT_T_INT16:
                            nBytes = ReadCompressedInt16Data(mat,matvar->internal->z,
                                 (mat_int16_t*)data->data,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT16:
                            nBytes = ReadCompressedUInt16Data(mat,matvar->internal->z,
                                 (mat_uint16_t*)data->data,packed_type,data->ndata);
                            break;
                        case MAT_T_INT8:
                            nBytes = ReadCompressedInt8Data(mat,matvar->internal->z,
                                 (mat_int8_t*)data->data,packed_type,data->ndata);
                            break;
                        case MAT_T_UINT8:
                            nBytes = ReadCompressedUInt8Data(mat,matvar->internal->z,
                                 (mat_uint8_t*)data->data,packed_type,data->ndata);
                            break;
                        default:
                            break;
                    }
#else   /* EXTENDED_SPARSE */
                    nBytes = ReadCompressedDoubleData(mat,matvar->internal->z,
                                 (double*)data->data,packed_type,data->ndata);
#endif   /* EXTENDED_SPARSE */
                    if ( data_in_tag )
                        nBytes+=4;
                    if ( (nBytes % 8) != 0 )
                        InflateSkip(mat,matvar->internal->z,8-(nBytes % 8));
#endif   /* HAVE_ZLIB */
                }
            }
            break;
        }
        case MAT_C_FUNCTION:
        {
            matvar_t **functions;
            size_t nfunctions = 0;

            if ( !matvar->nbytes || !matvar->data_size )
                break;
            nfunctions = matvar->nbytes / matvar->data_size;
            functions = (matvar_t **)matvar->data;
            if ( NULL != functions ) {
                size_t i;
                for ( i = 0; i < nfunctions; i++ ) {
                    Mat_VarRead5(mat,functions[i]);
                }
            }
            /* FIXME: */
            matvar->data_type = MAT_T_FUNCTION;
            break;
        }
        default:
            Mat_Critical("Mat_VarRead5: %d is not a supported class", matvar->class_type);
    }
    switch ( matvar->class_type ) {
        case MAT_C_DOUBLE:
        case MAT_C_SINGLE:
#ifdef HAVE_MAT_INT64_T
        case MAT_C_INT64:
#endif
#ifdef HAVE_MAT_UINT64_T
        case MAT_C_UINT64:
#endif
        case MAT_C_INT32:
        case MAT_C_UINT32:
        case MAT_C_INT16:
        case MAT_C_UINT16:
        case MAT_C_INT8:
        case MAT_C_UINT8:
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data;

                err = SafeMul(&matvar->nbytes, nelems, matvar->data_size);
                if ( err ) {
                    Mat_Critical("Integer multiplication overflow");
                    break;
                }

                complex_data = ComplexMalloc(matvar->nbytes);
                if ( NULL == complex_data ) {
                    Mat_Critical("Couldn't allocate memory for the complex data");
                    break;
                }

                Mat_VarReadNumeric5(mat,matvar,complex_data->Re,nelems);
                Mat_VarReadNumeric5(mat,matvar,complex_data->Im,nelems);
                matvar->data = complex_data;
            } else {
                err = SafeMul(&matvar->nbytes, nelems, matvar->data_size);
                if ( err ) {
                    Mat_Critical("Integer multiplication overflow");
                    break;
                }

                matvar->data = malloc(matvar->nbytes);
                if ( NULL == matvar->data ) {
                    Mat_Critical("Couldn't allocate memory for the data");
                    break;
                }
                Mat_VarReadNumeric5(mat,matvar,matvar->data,nelems);
            }
        default:
            break;
    }
    (void)fseek((FILE*)mat->fp,fpos,SEEK_SET);

    return;
}