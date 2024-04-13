gs_main_add_lib_path(gs_main_instance * minst, const char *lpath)
{
    /* Account for the possibility that the first element */
    /* is gp_current_directory name added by set_lib_paths. */
    int first_is_here =
        (r_size(&minst->lib_path.list) != 0 &&
         minst->lib_path.container.value.refs[0].value.bytes ==
         (const byte *)gp_current_directory_name ? 1 : 0);
    int code;

    r_set_size(&minst->lib_path.list, minst->lib_path.count +
               first_is_here);
    code = file_path_add(minst, &minst->lib_path, lpath);
    minst->lib_path.count = r_size(&minst->lib_path.list) - first_is_here;
    if (code < 0)
        return code;
    return gs_main_set_lib_paths(minst);
}