Mat_GetFilename(mat_t *mat)
{
    const char *filename = NULL;
    if ( NULL != mat )
        filename = mat->filename;
    return filename;
}