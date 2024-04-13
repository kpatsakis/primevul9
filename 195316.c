Mat_Close( mat_t *mat )
{
    int err = 0;

    if ( NULL != mat ) {
#if defined(MAT73) && MAT73
        if ( mat->version == 0x0200 ) {
            if ( mat->refs_id > -1 )
                H5Gclose(mat->refs_id);
            if ( 0 > H5Fclose(*(hid_t*)mat->fp) )
                err = 1;
            free(mat->fp);
            mat->fp = NULL;
        }
#endif
        if ( NULL != mat->fp )
            fclose((FILE*)mat->fp);
        if ( NULL != mat->header )
            free(mat->header);
        if ( NULL != mat->subsys_offset )
            free(mat->subsys_offset);
        if ( NULL != mat->filename )
            free(mat->filename);
        if ( NULL != mat->dir ) {
            size_t i;
            for ( i = 0; i < mat->num_datasets; i++ ) {
                if ( NULL != mat->dir[i] )
                    free(mat->dir[i]);
            }
            free(mat->dir);
        }
        free(mat);
    }

    return err;
}