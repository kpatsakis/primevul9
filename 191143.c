extend_path_list_container (gs_main_instance * minst, gs_file_path * pfp)
{
    uint len = r_size(&minst->lib_path.container);
    ref *paths, *opaths = minst->lib_path.container.value.refs;

    /* Add 5 entries at a time to reduce VM thrashing */
    paths = (ref *) gs_alloc_byte_array(minst->heap, len + LIB_PATH_EXTEND, sizeof(ref),
                                        "extend_path_list_container array");

    if (paths == 0) {
        return_error(gs_error_VMerror);
    }
    make_array(&minst->lib_path.container, avm_foreign, len + LIB_PATH_EXTEND, paths);
    make_array(&minst->lib_path.list, avm_foreign | a_readonly, 0,
               minst->lib_path.container.value.refs);

    memcpy(paths, opaths, len * sizeof(ref));
    r_set_size(&minst->lib_path.list, len);

    gs_free_object (minst->heap, opaths, "extend_path_list_container");
    return(0);
}