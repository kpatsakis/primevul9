gs_main_run_string_with_length(gs_main_instance * minst, const char *str,
         uint length, int user_errors, int *pexit_code, ref * perror_object)
{
    int code;

    code = gs_main_run_string_begin(minst, user_errors,
                                    pexit_code, perror_object);
    if (code < 0)
        return code;
    code = gs_main_run_string_continue(minst, str, length, user_errors,
                                       pexit_code, perror_object);
    if (code != gs_error_NeedInput)
        return code;
    return gs_main_run_string_end(minst, user_errors,
                                  pexit_code, perror_object);
}