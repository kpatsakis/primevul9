gs_main_run_string_continue(gs_main_instance * minst, const char *str,
         uint length, int user_errors, int *pexit_code, ref * perror_object)
{
    ref rstr;

    if (length == 0)
        return 0;               /* empty string signals EOF */
    make_const_string(&rstr, avm_foreign | a_readonly, length,
                      (const byte *)str);
    return gs_main_interpret(minst, &rstr, user_errors, pexit_code,
                        perror_object);
}