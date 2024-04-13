gs_main_init2(gs_main_instance * minst)
{
    i_ctx_t *i_ctx_p;
    int code = gs_main_init1(minst);
    int initial_init_level = minst->init_done;

    if (code < 0)
        return code;

    code = gs_main_init2aux(minst);
    if (code < 0)
       return code;
    i_ctx_p = minst->i_ctx_p; /* display_set_callback or run_string may change it */

    /* Now process the initial saved-pages=... argument, if any as well as saved-pages-test */
    if (initial_init_level < 2) {
       gx_device *pdev = gs_currentdevice(minst->i_ctx_p->pgs);	/* get the current device */
       gx_device_printer *ppdev = (gx_device_printer *)pdev;

        if (minst->saved_pages_test_mode) {
            if ((dev_proc(pdev, dev_spec_op)(pdev, gxdso_supports_saved_pages, NULL, 0) <= 0)) {
                /* no warning or error if saved-pages-test mode is used, just disable it */
                minst->saved_pages_test_mode = false;  /* device doesn't support it */
            } else {
                if ((code = gx_saved_pages_param_process(ppdev, (byte *)"begin", 5)) < 0)
                    return code;
                if (code > 0)
                    if ((code = gs_erasepage(minst->i_ctx_p->pgs)) < 0)
                        return code;
            }
        } else if (minst->saved_pages_initial_arg != NULL) {
            if (dev_proc(pdev, dev_spec_op)(pdev, gxdso_supports_saved_pages, NULL, 0) <= 0) {
                outprintf(minst->heap,
                          "   --saved-pages not supported by the '%s' device.\n",
                          pdev->dname);
                return gs_error_Fatal;
            }
            code = gx_saved_pages_param_process(ppdev, (byte *)minst->saved_pages_initial_arg,
                                                strlen(minst->saved_pages_initial_arg));
            if (code > 0)
                if ((code = gs_erasepage(minst->i_ctx_p->pgs)) < 0)
                    return code;
        }
    }

    if (gs_debug_c(':'))
        print_resource_usage(minst, &gs_imemory, "Start");
    gp_readline_init(&minst->readline_data, imemory_system);
    return 0;
}