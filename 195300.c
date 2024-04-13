Mat_VarReadDataAll(mat_t *mat,matvar_t *matvar)
{
    int err = 0;

    if ( mat == NULL || matvar == NULL )
        err = 1;
    else
        err = ReadData(mat,matvar);

    return err;
}