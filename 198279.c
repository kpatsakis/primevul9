ReadNextFunctionHandle(mat_t *mat, matvar_t *matvar)
{
    int err;
    size_t nelems = 1;

    err = SafeMulDims(matvar, &nelems);
    matvar->data_size = sizeof(matvar_t *);
    err |= SafeMul(&matvar->nbytes, nelems, matvar->data_size);
    if ( err )
        return 0;

    matvar->data = malloc(matvar->nbytes);
    if ( matvar->data != NULL ) {
        size_t i;
        matvar_t **functions = (matvar_t**)matvar->data;;
        for ( i = 0; i < nelems; i++ )
            functions[i] = Mat_VarReadNextInfo(mat);
    } else {
        matvar->data_size = 0;
        matvar->nbytes    = 0;
    }

    return 0;
}