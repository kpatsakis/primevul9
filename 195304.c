Mat_GetDir(mat_t *mat, size_t *n)
{
    char ** dir = NULL;

    if ( NULL == n )
        return dir;

    if ( NULL == mat ) {
        *n = 0;
        return dir;
    }

    if ( NULL == mat->dir ) {
        matvar_t *matvar = NULL;

        if ( mat->version == MAT_FT_MAT73 ) {
            size_t i = 0;
            size_t fpos = mat->next_index;
            if ( mat->num_datasets == 0 ) {
                *n = 0;
                return dir;
            }
            mat->dir = (char**)calloc(mat->num_datasets, sizeof(char*));
            if ( NULL == mat->dir ) {
                *n = 0;
                Mat_Critical("Couldn't allocate memory for the directory");
                return dir;
            }
            mat->next_index = 0;
            while ( mat->next_index < mat->num_datasets ) {
                matvar = Mat_VarReadNextInfo(mat);
                if ( NULL != matvar ) {
                    if ( NULL != matvar->name ) {
                        mat->dir[i++] = strdup_printf("%s",
                            matvar->name);
                    }
                    Mat_VarFree(matvar);
                } else {
                    Mat_Critical("An error occurred in reading the MAT file");
                    break;
                }
            }
            mat->next_index = fpos;
            *n = i;
        } else {
            long fpos = ftell((FILE*)mat->fp);
            if ( fpos == -1L ) {
                *n = 0;
                Mat_Critical("Couldn't determine file position");
                return dir;
            }
            (void)fseek((FILE*)mat->fp,mat->bof,SEEK_SET);
            mat->num_datasets = 0;
            do {
                matvar = Mat_VarReadNextInfo(mat);
                if ( NULL != matvar ) {
                    if ( NULL != matvar->name ) {
                        if ( NULL == mat->dir ) {
                            dir = (char**)malloc(sizeof(char*));
                        } else {
                            dir = (char**)realloc(mat->dir,
                                (mat->num_datasets + 1)*(sizeof(char*)));
                        }
                        if ( NULL != dir ) {
                            mat->dir = dir;
                            mat->dir[mat->num_datasets++] =
                                strdup_printf("%s", matvar->name);
                        } else {
                            Mat_Critical("Couldn't allocate memory for the directory");
                            break;
                        }
                    }
                    Mat_VarFree(matvar);
                } else if ( !feof((FILE *)mat->fp) ) {
                    Mat_Critical("An error occurred in reading the MAT file");
                    break;
                }
            } while ( !feof((FILE *)mat->fp) );
            (void)fseek((FILE*)mat->fp,fpos,SEEK_SET);
            *n = mat->num_datasets;
        }
    } else {
        if ( mat->version == MAT_FT_MAT73 ) {
            *n = 0;
            while ( *n < mat->num_datasets && NULL != mat->dir[*n] ) {
                (*n)++;
            }
        } else {
            *n = mat->num_datasets;
        }
    }
    dir = mat->dir;
    return dir;
}