gs_run_init_file(gs_main_instance * minst, int *pexit_code, ref * perror_object)
{
    i_ctx_t *i_ctx_p = minst->i_ctx_p;
    ref ifile;
    ref first_token;
    int code;
    scanner_state state;

    gs_main_set_lib_paths(minst);
    code = gs_main_run_file_open(minst, gs_init_file, &ifile);
    if (code < 0) {
        *pexit_code = 255;
        return code;
    }
    /* Check to make sure the first token is an integer */
    /* (for the version number check.) */
    gs_scanner_init(&state, &ifile);
    code = gs_scan_token(i_ctx_p, &first_token, &state);
    if (code != 0 || !r_has_type(&first_token, t_integer)) {
        emprintf1(minst->heap,
                  "Initialization file %s does not begin with an integer.\n",
                  gs_init_file);
        *pexit_code = 255;
        return_error(gs_error_Fatal);
    }
    *++osp = first_token;
    r_set_attrs(&ifile, a_executable);
    return gs_main_interpret(minst, &ifile, minst->user_errors,
                        pexit_code, perror_object);
}