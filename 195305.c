Mat_VarWrite(mat_t *mat,matvar_t *matvar,enum matio_compression compress)
{
    int err;

    if ( NULL == mat || NULL == matvar )
        return -1;

    if ( NULL == mat->dir ) {
        size_t n = 0;
        (void)Mat_GetDir(mat, &n);
    }

    {
        /* Error if MAT variable already exists in MAT file */
        size_t i;
        for ( i = 0; i < mat->num_datasets; i++ ) {
            if ( NULL != mat->dir[i] &&
                0 == strcmp(mat->dir[i], matvar->name) ) {
                Mat_Critical("Variable %s already exists.", matvar->name);
                return 1;
            }
        }
    }

    if ( mat->version == MAT_FT_MAT5 )
        err = Mat_VarWrite5(mat,matvar,compress);
    else if ( mat->version == MAT_FT_MAT73 )
#if defined(MAT73) && MAT73
        err = Mat_VarWrite73(mat,matvar,compress);
#else
        err = 1;
#endif
    else if ( mat->version == MAT_FT_MAT4 )
        err = Mat_VarWrite4(mat,matvar);
    else
        err = 2;

    if ( err == 0 ) {
        /* Update directory */
        char **dir;
        if ( NULL == mat->dir ) {
            dir = (char**)malloc(sizeof(char*));
        } else {
            dir = (char**)realloc(mat->dir,
            (mat->num_datasets + 1)*(sizeof(char*)));
        }
        if ( NULL != dir ) {
            mat->dir = dir;
            if ( NULL != matvar->name ) {
                mat->dir[mat->num_datasets++] =
                    strdup_printf("%s", matvar->name);
            } else {
                mat->dir[mat->num_datasets++] = NULL;
            }
        } else {
            err = 3;
            Mat_Critical("Couldn't allocate memory for the directory");
        }
    }

    return err;
}