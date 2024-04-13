GetEmptyMatrixMaxBufSize(const char *name, int rank, size_t *size)
{
    int err = 0;
    size_t nBytes = 0, len, rank_size;
    size_t tag_size = 8, array_flags_size = 8;

    /* Add the Array Flags tag and space to the number of bytes */
    nBytes += tag_size + array_flags_size;

    /* Get size of variable name, pad it to an 8 byte block, and add it to nBytes */
    if ( NULL != name )
        len = strlen(name);
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

    /* Add rank and dimensions, padded to an 8 byte block */
    err |= SafeMul(&rank_size, rank, 4);
    if ( rank % 2 )
        err |= SafeAdd(&nBytes, nBytes, tag_size + 4);
    else
        err |= SafeAdd(&nBytes, nBytes, tag_size);

    err |= SafeAdd(&nBytes, nBytes, rank_size);
    /* Data tag */
    err |= SafeAdd(&nBytes, nBytes, tag_size);

    if ( err )
        return 1;

    *size = nBytes;
    return 0;
}