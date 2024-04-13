gs_main_set_lib_paths(gs_main_instance * minst)
{
    ref *paths = minst->lib_path.container.value.refs;
    int first_is_here =
        (r_size(&minst->lib_path.list) != 0 &&
         paths[0].value.bytes == (const byte *)gp_current_directory_name ? 1 : 0);
    int code = 0;
    int count = minst->lib_path.count;
    int i, have_rom_device = 0;

    if (minst->search_here_first) {
        if (!(first_is_here ||
              (r_size(&minst->lib_path.list) != 0 &&
               !bytes_compare((const byte *)gp_current_directory_name,
                              strlen(gp_current_directory_name),
                              paths[0].value.bytes,
                              r_size(&paths[0]))))
            ) {
            memmove(paths + 1, paths, count * sizeof(*paths));
            make_const_string(paths, avm_foreign | a_readonly,
                              strlen(gp_current_directory_name),
                              (const byte *)gp_current_directory_name);
        }
    } else {
        if (first_is_here)
            memmove(paths, paths + 1, count * sizeof(*paths));
    }
    r_set_size(&minst->lib_path.list,
               count + (minst->search_here_first ? 1 : 0));
    if (minst->lib_path.env != 0)
        code = file_path_add(minst, &minst->lib_path, minst->lib_path.env);
    /* now put the %rom%lib/ device path before the gs_lib_default_path on the list */
    for (i = 0; i < gx_io_device_table_count; i++) {
        const gx_io_device *iodev = gx_io_device_table[i];
        const char *dname = iodev->dname;

        if (dname && strlen(dname) == 5 && !memcmp("%rom%", dname, 5)) {
            struct stat pstat;
            /* gs_error_unregistered means no usable romfs is available */
            int code = iodev->procs.file_status((gx_io_device *)iodev, dname, &pstat);
            if (code != gs_error_unregistered){
                have_rom_device = 1;
            }
            break;
        }
    }
    if (have_rom_device && code >= 0) {
        code = file_path_add(minst, &minst->lib_path, "%rom%Resource/Init/");
        if (code < 0)
            return code;
        code = file_path_add(minst, &minst->lib_path, "%rom%lib/");
    }
    if (minst->lib_path.final != 0 && code >= 0)
        code = file_path_add(minst, &minst->lib_path, minst->lib_path.final);
    return code;
}