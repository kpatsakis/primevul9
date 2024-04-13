Mat_VarRead( mat_t *mat, const char *name )
{
    matvar_t *matvar = NULL;

    if ( (mat == NULL) || (name == NULL) )
        return NULL;

    if ( MAT_FT_MAT73 != mat->version ) {
        long fpos = ftell((FILE*)mat->fp);
        if ( fpos == -1L ) {
            Mat_Critical("Couldn't determine file position");
            return NULL;
        }
        matvar = Mat_VarReadInfo(mat,name);
        if ( matvar ) {
            const int err = ReadData(mat,matvar);
            if ( err ) {
                Mat_VarFree(matvar);
                matvar = NULL;
            }
        }
        (void)fseek((FILE*)mat->fp,fpos,SEEK_SET);
    } else {
        size_t fpos = mat->next_index;
        mat->next_index = 0;
        matvar = Mat_VarReadInfo(mat,name);
        if ( matvar ) {
            const int err = ReadData(mat,matvar);
            if ( err ) {
                Mat_VarFree(matvar);
                matvar = NULL;
            }
        }
        mat->next_index = fpos;
    }

    return matvar;
}