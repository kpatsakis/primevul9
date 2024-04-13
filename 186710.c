static char **concatenate_string_array(TALLOC_CTX *mem_ctx,
                                       char **arr1, size_t len1,
                                       char **arr2, size_t len2)
{
    size_t i, j;
    size_t new_size = len1 + len2;
    char ** string_array = talloc_realloc(mem_ctx, arr1, char *, new_size + 1);
    if (string_array == NULL) {
        return NULL;
    }

    for (i=len1, j=0; i < new_size; ++i,++j) {
        string_array[i] = talloc_steal(string_array,
                                       arr2[j]);
    }

    string_array[i] = NULL;

    return string_array;
}