WriteCharData(mat_t *mat, void *data, int N,enum matio_types data_type)
{
    int nBytes = 0, i;
    size_t byteswritten = 0;
    mat_int8_t pad1 = 0;

    switch ( data_type ) {
        case MAT_T_UINT16:
        {
            nBytes = N*2;
            fwrite(&data_type,4,1,(FILE*)mat->fp);
            fwrite(&nBytes,4,1,(FILE*)mat->fp);
            if ( NULL != data && N > 0 )
                fwrite(data,2,N,(FILE*)mat->fp);
            if ( nBytes % 8 )
                for ( i = nBytes % 8; i < 8; i++ )
                    fwrite(&pad1,1,1,(FILE*)mat->fp);
            break;
        }
        case MAT_T_INT8:
        case MAT_T_UINT8:
        {
            mat_uint8_t *ptr;
            mat_uint16_t c;

            /* Matlab can't read MAT_C_CHAR as uint8, needs uint16 */
            nBytes = N*2;
            data_type = MAT_T_UINT16;
            fwrite(&data_type,4,1,(FILE*)mat->fp);
            fwrite(&nBytes,4,1,(FILE*)mat->fp);
            ptr = (mat_uint8_t*)data;
            if ( NULL == ptr )
                break;
            for ( i = 0; i < N; i++ ) {
                c = (mat_uint16_t)*(char *)ptr;
                fwrite(&c,2,1,(FILE*)mat->fp);
                ptr++;
            }
            if ( nBytes % 8 )
                for ( i = nBytes % 8; i < 8; i++ )
                    fwrite(&pad1,1,1,(FILE*)mat->fp);
            break;
        }
        case MAT_T_UTF8:
        {
            mat_uint8_t *ptr;

            nBytes = N;
            fwrite(&data_type,4,1,(FILE*)mat->fp);
            fwrite(&nBytes,4,1,(FILE*)mat->fp);
            ptr = (mat_uint8_t*)data;
            if ( NULL != ptr && nBytes > 0 )
                fwrite(ptr,1,nBytes,(FILE*)mat->fp);
            if ( nBytes % 8 )
                for ( i = nBytes % 8; i < 8; i++ )
                    fwrite(&pad1,1,1,(FILE*)mat->fp);
            break;
        }
        case MAT_T_UNKNOWN:
        {
            /* Sometimes empty char data will have MAT_T_UNKNOWN, so just write
             * a data tag
             */
            nBytes = N*2;
            data_type = MAT_T_UINT16;
            fwrite(&data_type,4,1,(FILE*)mat->fp);
            fwrite(&nBytes,4,1,(FILE*)mat->fp);
            break;
        }
        default:
            break;
    }
    byteswritten += nBytes;
    return byteswritten;
}