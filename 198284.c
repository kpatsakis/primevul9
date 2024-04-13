WriteData(mat_t *mat,void *data,int N,enum matio_types data_type)
{
    int nBytes = 0, data_size;

    if ( mat == NULL || mat->fp == NULL )
        return 0;

    data_size = Mat_SizeOf(data_type);
    nBytes    = N*data_size;
    fwrite(&data_type,4,1,(FILE*)mat->fp);
    fwrite(&nBytes,4,1,(FILE*)mat->fp);

    if ( data != NULL && N > 0 )
        fwrite(data,data_size,N,(FILE*)mat->fp);

    return nBytes;
}