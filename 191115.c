gs_main_run_string_end(gs_main_instance * minst, int user_errors,
                       int *pexit_code, ref * perror_object)
{
    ref rstr;

    make_empty_const_string(&rstr, avm_foreign | a_readonly);
    return gs_main_interpret(minst, &rstr, user_errors, pexit_code,
                        perror_object);
}