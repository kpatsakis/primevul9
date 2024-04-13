gs_main_run_file_open(gs_main_instance * minst, const char *file_name, ref * pfref)
{
    gs_main_set_lib_paths(minst);
    if (gs_main_lib_open(minst, file_name, pfref) < 0) {
        emprintf1(minst->heap,
                  "Can't find initialization file %s.\n",
                  file_name);
        return_error(gs_error_Fatal);
    }
    r_set_attrs(pfref, a_execute + a_executable);
    return 0;
}