Mat_GetHeader(mat_t *mat)
{
    const char *header = NULL;
    if ( NULL != mat )
        header = mat->header;
    return header;
}