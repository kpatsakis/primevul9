Mat_VarReadInfo( mat_t *mat, const char *name )
{
    matvar_t *matvar = NULL;

    if ( (mat == NULL) || (name == NULL) )
        return NULL;

    if ( mat->version == MAT_FT_MAT73 ) {
        size_t fpos = mat->next_index;
        mat->next_index = 0;
        while ( NULL == matvar && mat->next_index < mat->num_datasets ) {
            matvar = Mat_VarReadNextInfo(mat);
            if ( matvar != NULL ) {
                if ( matvar->name == NULL || 0 != strcmp(matvar->name,name) ) {
                    Mat_VarFree(matvar);
                    matvar = NULL;
                }
            } else {
                Mat_Critical("An error occurred in reading the MAT file");
                break;
            }
        }
        mat->next_index = fpos;
    } else {
        long fpos = ftell((FILE*)mat->fp);
        if ( fpos != -1L ) {
            (void)fseek((FILE*)mat->fp,mat->bof,SEEK_SET);
            do {
                matvar = Mat_VarReadNextInfo(mat);
                if ( matvar != NULL ) {
                    if ( matvar->name == NULL || 0 != strcmp(matvar->name,name) ) {
                        Mat_VarFree(matvar);
                        matvar = NULL;
                    }
                } else if ( !feof((FILE *)mat->fp) ) {
                    Mat_Critical("An error occurred in reading the MAT file");
                    break;
                }
            } while ( NULL == matvar && !feof((FILE *)mat->fp) );
            (void)fseek((FILE*)mat->fp,fpos,SEEK_SET);
        } else {
            Mat_Critical("Couldn't determine file position");
        }
    }

    return matvar;
}