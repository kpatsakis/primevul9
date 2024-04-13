Mat_VarWriteAppend(mat_t *mat,matvar_t *matvar,enum matio_compression compress,int dim)
{
    int err;

    if ( NULL == mat || NULL == matvar )
        return -1;

    if ( NULL == mat->dir ) {
        size_t n = 0;
        (void)Mat_GetDir(mat, &n);
    }

    if ( mat->version == MAT_FT_MAT73 ) {
#if defined(MAT73) && MAT73
        int append = 0;
        {
            /* Check if MAT variable already exists in MAT file */
            size_t i;
            for ( i = 0; i < mat->num_datasets; i++ ) {
                if ( NULL != mat->dir[i] &&
                    0 == strcmp(mat->dir[i], matvar->name) ) {
                    append = 1;
                    break;
                }
            }
        }
        err = Mat_VarWriteAppend73(mat,matvar,compress,dim);
        if ( err == 0 && 0 == append ) {
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
#else
        err = 1;
#endif
    }
    else
        err = 2;

    return err;
}