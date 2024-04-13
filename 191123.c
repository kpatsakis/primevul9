gs_main_run_file(gs_main_instance * minst, const char *file_name, int user_errors, int *pexit_code, ref * perror_object)
{
    ref initial_file;
    int code = gs_main_run_file_open(minst, file_name, &initial_file);

    if (code < 0)
        return code;
    return gs_main_interpret(minst, &initial_file, user_errors,
                        pexit_code, perror_object);
}