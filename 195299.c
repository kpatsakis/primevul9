Mat_Rewind( mat_t *mat )
{
    int err = 0;

    switch ( mat->version ) {
        case MAT_FT_MAT5:
            (void)fseek((FILE*)mat->fp,128L,SEEK_SET);
            break;
        case MAT_FT_MAT73:
            mat->next_index = 0;
            break;
        case MAT_FT_MAT4:
            (void)fseek((FILE*)mat->fp,0L,SEEK_SET);
            break;
        default:
            err = -1;
            break;
    }

    return err;
}