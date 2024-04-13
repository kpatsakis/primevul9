ComplexFree(mat_complex_split_t* complex_data)
{
    free(complex_data->Re);
    free(complex_data->Im);
    free(complex_data);
}