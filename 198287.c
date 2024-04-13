GetMatrixMaxBufSize(matvar_t *matvar, size_t *size)
{
    int err = 0;
    size_t nBytes = 0, len, type_buf_size;
    size_t tag_size = 8, array_flags_size = 8;

    if ( matvar == NULL )
        return 1;

    /* Add the Array Flags tag and space to the number of bytes */
    nBytes += tag_size + array_flags_size;

    /* Get size of variable name, pad it to an 8 byte block, and add it to nBytes */
    if ( NULL != matvar->name )
        len = strlen(matvar->name);
    else
        len = 4;

    if ( len <= 4 ) {
        nBytes += tag_size;
    } else {
        nBytes += tag_size;
        if ( len % 8 )
            err |= SafeAdd(&len, len, 8 - len % 8);

        err |= SafeAdd(&nBytes, nBytes, len);
    }

    err |= GetTypeBufSize(matvar, &type_buf_size);
    err |= SafeAdd(&nBytes, nBytes, type_buf_size);

    if ( err )
        return 1;

    *size = nBytes;
    return 0;
}