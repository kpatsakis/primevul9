Mat_VarCalloc(void)
{
    matvar_t *matvar;

    matvar = (matvar_t*)malloc(sizeof(*matvar));

    if ( NULL != matvar ) {
        matvar->nbytes       = 0;
        matvar->rank         = 0;
        matvar->data_type    = MAT_T_UNKNOWN;
        matvar->data_size    = 0;
        matvar->class_type   = MAT_C_EMPTY;
        matvar->isComplex    = 0;
        matvar->isGlobal     = 0;
        matvar->isLogical    = 0;
        matvar->dims         = NULL;
        matvar->name         = NULL;
        matvar->data         = NULL;
        matvar->mem_conserve = 0;
        matvar->compression  = MAT_COMPRESSION_NONE;
        matvar->internal     = (struct matvar_internal*)malloc(sizeof(*matvar->internal));
        if ( NULL == matvar->internal ) {
            free(matvar);
            matvar = NULL;
        } else {
#if defined(MAT73) && MAT73
            matvar->internal->hdf5_name  = NULL;
            matvar->internal->hdf5_ref   =  0;
            matvar->internal->id         = -1;
#endif
            matvar->internal->datapos    = 0;
            matvar->internal->num_fields = 0;
            matvar->internal->fieldnames = NULL;
#if HAVE_ZLIB
            matvar->internal->z          = NULL;
            matvar->internal->data       = NULL;
#endif
        }
    }

    return matvar;
}