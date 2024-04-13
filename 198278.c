SetFieldNames(matvar_t *matvar, char *buf, size_t nfields, mat_uint32_t fieldname_length)
{
    size_t i;
    matvar->internal->num_fields = nfields;
    matvar->internal->fieldnames =
        (char**)calloc(nfields,sizeof(*matvar->internal->fieldnames));
    if ( NULL != matvar->internal->fieldnames ) {
        for ( i = 0; i < nfields; i++ ) {
            matvar->internal->fieldnames[i] = (char*)malloc(fieldname_length);
            if ( NULL != matvar->internal->fieldnames[i] ) {
                memcpy(matvar->internal->fieldnames[i], buf+i*fieldname_length, fieldname_length);
                matvar->internal->fieldnames[i][fieldname_length-1] = '\0';
            }
        }
    }
}