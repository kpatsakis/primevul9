Mat_VarReadNext( mat_t *mat )
{
    long fpos = 0;
    matvar_t *matvar = NULL;

    if ( mat->version != MAT_FT_MAT73 ) {
        if ( feof((FILE *)mat->fp) )
            return NULL;
        /* Read position so we can reset the file position if an error occurs */
        fpos = ftell((FILE*)mat->fp);
        if ( fpos == -1L ) {
            Mat_Critical("Couldn't determine file position");
            return NULL;
        }
    }
    matvar = Mat_VarReadNextInfo(mat);
    if ( matvar ) {
        const int err = ReadData(mat,matvar);
        if ( err ) {
            Mat_VarFree(matvar);
            matvar = NULL;
        }
    } else if ( mat->version != MAT_FT_MAT73 ) {
        (void)fseek((FILE*)mat->fp,fpos,SEEK_SET);
    }

    return matvar;
}