GetStructFieldBufSize(matvar_t *matvar, size_t *size)
{
    int err;
    size_t nBytes = 0, type_buf_size;
    size_t tag_size = 8, array_flags_size = 8;

    *size = 0;

    if ( matvar == NULL )
        return GetEmptyMatrixMaxBufSize(NULL, 2, size);

    /* Add the Array Flags tag and space to the number of bytes */
    nBytes += tag_size + array_flags_size;

    /* In a struct field, the name is just a tag with 0 bytes */
    nBytes += tag_size;

    err = GetTypeBufSize(matvar, &type_buf_size);
    err |= SafeAdd(&nBytes, nBytes, type_buf_size);
    if ( err )
        return 1;

    *size = nBytes;
    return 0;
}