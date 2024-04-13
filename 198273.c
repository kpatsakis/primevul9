Mat_VarReadNumeric5(mat_t *mat,matvar_t *matvar,void *data,size_t N)
{
    int nBytes = 0, data_in_tag = 0;
    enum matio_types packed_type = MAT_T_UNKNOWN;
    mat_uint32_t tag[2];

    if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
#if defined(HAVE_ZLIB)
        matvar->internal->z->avail_in = 0;
        InflateDataType(mat,matvar->internal->z,tag);
        if ( mat->byteswap )
            (void)Mat_uint32Swap(tag);

        packed_type = TYPE_FROM_TAG(tag[0]);
        if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
            data_in_tag = 1;
            nBytes = (tag[0] & 0xffff0000) >> 16;
        } else {
            data_in_tag = 0;
            InflateDataType(mat,matvar->internal->z,tag+1);
            if ( mat->byteswap )
                (void)Mat_uint32Swap(tag+1);
            nBytes = tag[1];
        }
#endif
    } else {
        size_t bytesread = fread(tag,4,1,(FILE*)mat->fp);
        if ( mat->byteswap )
            (void)Mat_uint32Swap(tag);
        packed_type = TYPE_FROM_TAG(tag[0]);
        if ( tag[0] & 0xffff0000 ) { /* Data is in the tag */
            data_in_tag = 1;
            nBytes = (tag[0] & 0xffff0000) >> 16;
        } else {
            data_in_tag = 0;
            bytesread += fread(tag+1,4,1,(FILE*)mat->fp);
            if ( mat->byteswap )
                (void)Mat_uint32Swap(tag+1);
            nBytes = tag[1];
        }
    }
    if ( nBytes == 0 ) {
        matvar->nbytes = 0;
        return;
    }

    if ( matvar->compression == MAT_COMPRESSION_NONE ) {
        switch ( matvar->class_type ) {
            case MAT_C_DOUBLE:
                nBytes = ReadDoubleData(mat,(double*)data,packed_type,N);
                break;
            case MAT_C_SINGLE:
                nBytes = ReadSingleData(mat,(float*)data,packed_type,N);
                break;
            case MAT_C_INT64:
#ifdef HAVE_MAT_INT64_T
                nBytes = ReadInt64Data(mat,(mat_int64_t*)data,packed_type,N);
#endif
                break;
            case MAT_C_UINT64:
#ifdef HAVE_MAT_UINT64_T
                nBytes = ReadUInt64Data(mat,(mat_uint64_t*)data,packed_type,N);
#endif
                break;
            case MAT_C_INT32:
                nBytes = ReadInt32Data(mat,(mat_int32_t*)data,packed_type,N);
                break;
            case MAT_C_UINT32:
                nBytes = ReadUInt32Data(mat,(mat_uint32_t*)data,packed_type,N);
                break;
            case MAT_C_INT16:
                nBytes = ReadInt16Data(mat,(mat_int16_t*)data,packed_type,N);
                break;
            case MAT_C_UINT16:
                nBytes = ReadUInt16Data(mat,(mat_uint16_t*)data,packed_type,N);
                break;
            case MAT_C_INT8:
                nBytes = ReadInt8Data(mat,(mat_int8_t*)data,packed_type,N);
                break;
            case MAT_C_UINT8:
                nBytes = ReadUInt8Data(mat,(mat_uint8_t*)data,packed_type,N);
                break;
            default:
                break;
        }
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
        switch ( matvar->class_type ) {
            case MAT_C_DOUBLE:
                nBytes = ReadCompressedDoubleData(mat,matvar->internal->z,(double*)data,
                                                  packed_type,N);
                break;
            case MAT_C_SINGLE:
                nBytes = ReadCompressedSingleData(mat,matvar->internal->z,(float*)data,
                                                  packed_type,N);
                break;
            case MAT_C_INT64:
#ifdef HAVE_MAT_INT64_T
                nBytes = ReadCompressedInt64Data(mat,matvar->internal->z,(mat_int64_t*)data,
                                                 packed_type,N);
#endif
                break;
            case MAT_C_UINT64:
#ifdef HAVE_MAT_UINT64_T
                nBytes = ReadCompressedUInt64Data(mat,matvar->internal->z,(mat_uint64_t*)data,
                                                  packed_type,N);
#endif
                break;
            case MAT_C_INT32:
                nBytes = ReadCompressedInt32Data(mat,matvar->internal->z,(mat_int32_t*)data,
                                                 packed_type,N);
                break;
            case MAT_C_UINT32:
                nBytes = ReadCompressedUInt32Data(mat,matvar->internal->z,(mat_uint32_t*)data,
                                                  packed_type,N);
                break;
            case MAT_C_INT16:
                nBytes = ReadCompressedInt16Data(mat,matvar->internal->z,(mat_int16_t*)data,
                                                 packed_type,N);
                break;
            case MAT_C_UINT16:
                nBytes = ReadCompressedUInt16Data(mat,matvar->internal->z,(mat_uint16_t*)data,
                                                  packed_type,N);
                break;
            case MAT_C_INT8:
                nBytes = ReadCompressedInt8Data(mat,matvar->internal->z,(mat_int8_t*)data,
                                                packed_type,N);
                break;
            case MAT_C_UINT8:
                nBytes = ReadCompressedUInt8Data(mat,matvar->internal->z,(mat_uint8_t*)data,
                                                 packed_type,N);
                break;
            default:
                break;
        }
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
}