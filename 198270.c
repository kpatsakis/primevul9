ReadSparse(mat_t *mat, matvar_t *matvar, int *n, mat_int32_t **v)
{
    int data_in_tag = 0;
    enum matio_types packed_type;
    mat_uint32_t tag[2];
    size_t bytesread = 0;
    mat_int32_t N = 0;

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
    *n = N / 4;
    *v = (mat_int32_t*)malloc(*n*sizeof(mat_int32_t));
    if ( NULL != *v ) {
        int nBytes;
        if ( matvar->compression == MAT_COMPRESSION_NONE ) {
            nBytes = ReadInt32Data(mat,*v,packed_type,*n);
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
            nBytes = ReadCompressedInt32Data(mat,matvar->internal->z,
                            *v,packed_type,*n);
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
    } else {
        Mat_Critical("Couldn't allocate memory");
    }

    return bytesread;
}