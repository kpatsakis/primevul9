gs_main_run_string(gs_main_instance * minst, const char *str, int user_errors,
                   int *pexit_code, ref * perror_object)
{
    return gs_main_run_string_with_length(minst, str, (uint) strlen(str),
                                          user_errors,
                                          pexit_code, perror_object);
}