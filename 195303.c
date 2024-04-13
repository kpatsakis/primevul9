ReadData(mat_t *mat, matvar_t *matvar)
{
    if ( mat == NULL || matvar == NULL || mat->fp == NULL )
        return 1;
    else if ( mat->version == MAT_FT_MAT5 )
        return Mat_VarRead5(mat,matvar);
#if defined(MAT73) && MAT73
    else if ( mat->version == MAT_FT_MAT73 )
        return Mat_VarRead73(mat,matvar);
#endif
    else if ( mat->version == MAT_FT_MAT4 )
        return Mat_VarRead4(mat,matvar);
    return 1;
}