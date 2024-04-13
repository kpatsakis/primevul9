Mat_GetVersion(mat_t *mat)
{
    enum mat_ft file_type = MAT_FT_UNDEFINED;
    if ( NULL != mat )
        file_type = (enum mat_ft)mat->version;
    return file_type;
}