file_path_add(gs_main_instance * minst, gs_file_path * pfp, const char *dirs)
{
    uint len = r_size(&pfp->list);
    const char *dpath = dirs;
    int code;

    if (dirs == 0)
        return 0;
    for (;;) {                  /* Find the end of the next directory name. */
        const char *npath = dpath;

        while (*npath != 0 && *npath != gp_file_name_list_separator)
            npath++;
        if (npath > dpath) {
            if (len == r_size(&pfp->container)) {
                code = extend_path_list_container(minst, pfp);
                if (code < 0) {
                    emprintf(minst->heap, "\nAdding path to search paths failed.\n");
                    return(code);
                }
            }
            make_const_string(&pfp->container.value.refs[len],
                              avm_foreign | a_readonly,
                              npath - dpath, (const byte *)dpath);
            ++len;
        }
        if (!*npath)
            break;
        dpath = npath + 1;
    }
    r_set_size(&pfp->list, len);
    return 0;
}