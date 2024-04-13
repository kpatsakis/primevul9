gs_main_init0(gs_main_instance * minst, FILE * in, FILE * out, FILE * err,
              int max_lib_paths)
{
    ref *array;

    /* Do platform-dependent initialization. */
    /* We have to do this as the very first thing, */
    /* because it detects attempts to run 80N86 executables (N>0) */
    /* on incompatible processors. */
    gp_init();

    /* Initialize the imager. */

    /* Reset debugging flags */
#ifdef PACIFY_VALGRIND
    VALGRIND_HG_DISABLE_CHECKING(gs_debug, 128);
#endif
    memset(gs_debug, 0, 128);
    gs_log_errors = 0;  /* gs_debug['#'] = 0 */

    gp_get_realtime(minst->base_time);

    /* Initialize the file search paths. */
    array = (ref *) gs_alloc_byte_array(minst->heap, max_lib_paths, sizeof(ref),
                                        "lib_path array");
    if (array == 0) {
        gs_lib_finit(1, gs_error_VMerror, minst->heap);
        return_error(gs_error_VMerror);
    }
    make_array(&minst->lib_path.container, avm_foreign, max_lib_paths,
               array);
    make_array(&minst->lib_path.list, avm_foreign | a_readonly, 0,
               minst->lib_path.container.value.refs);
    minst->lib_path.env = 0;
    minst->lib_path.final = 0;
    minst->lib_path.count = 0;
    minst->user_errors = 1;
    minst->init_done = 0;
    return 0;
}