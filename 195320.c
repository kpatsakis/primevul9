ComplexMalloc(size_t nbytes)
{
    mat_complex_split_t *complex_data = (mat_complex_split_t*)malloc(sizeof(*complex_data));
    if ( NULL != complex_data ) {
        complex_data->Re = malloc(nbytes);
        if ( NULL != complex_data->Re ) {
            complex_data->Im = malloc(nbytes);
            if ( NULL == complex_data->Im ) {
                free(complex_data->Re);
                free(complex_data);
                complex_data = NULL;
            }
        }
        else {
            free(complex_data);
            complex_data = NULL;
        }
    }

    return complex_data;
}