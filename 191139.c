gs_main_run_string_begin(gs_main_instance * minst, int user_errors,
                         int *pexit_code, ref * perror_object)
{
    const char *setup = ".runstringbegin";
    ref rstr;
    int code;

    gs_main_set_lib_paths(minst);
    make_const_string(&rstr, avm_foreign | a_readonly | a_executable,
                      strlen(setup), (const byte *)setup);
    code = gs_main_interpret(minst, &rstr, user_errors, pexit_code,
                        perror_object);
    return (code == gs_error_NeedInput ? 0 : code == 0 ? gs_error_Fatal : code);
}