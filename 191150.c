gs_main_interpret(gs_main_instance *minst, ref * pref, int user_errors,
        int *pexit_code, ref * perror_object)
{
    int code;

    /* set interpreter pointer to lib_path */
    minst->i_ctx_p->lib_path = &minst->lib_path;

    code = gs_interpret(&minst->i_ctx_p, pref,
                user_errors, pexit_code, perror_object);
    return code;
}