Mat_VarReadNextInfo( mat_t *mat )
{
    matvar_t *matvar;
    if ( mat == NULL )
        return NULL;

    switch ( mat->version ) {
        case MAT_FT_MAT5:
            matvar = Mat_VarReadNextInfo5(mat);
            break;
        case MAT_FT_MAT73:
#if defined(MAT73) && MAT73
            matvar = Mat_VarReadNextInfo73(mat);
#else
            matvar = NULL;
#endif
            break;
        case MAT_FT_MAT4:
            matvar = Mat_VarReadNextInfo4(mat);
            break;
        default:
            matvar = NULL;
            break;
    }

    return matvar;
}