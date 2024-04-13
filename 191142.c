gs_main_init1(gs_main_instance * minst)
{
    gs_dual_memory_t idmem;
    name_table *nt = NULL;
    int code = 0;

    if (minst->init_done < 1) {
        code = ialloc_init(&idmem, minst->heap,
                        minst->memory_clump_size, gs_have_level2());

        if (code < 0)
            return code;
        code = gs_lib_init1((gs_memory_t *)idmem.space_system);
        if (code < 0)
            goto fail;
        alloc_save_init(&idmem);
        {
            gs_memory_t *mem = (gs_memory_t *)idmem.space_system;
            nt = names_init(minst->name_table_size, idmem.space_system);

            if (nt == 0) {
                code = gs_note_error(gs_error_VMerror);
                goto fail;
            }
            mem->gs_lib_ctx->gs_name_table = nt;
            code = gs_register_struct_root(mem, mem->gs_lib_ctx->name_table_root,
                                           (void **)&mem->gs_lib_ctx->gs_name_table,
                                           "the_gs_name_table");
            if (code < 0)
                goto fail;
            mem->gs_lib_ctx->client_check_file_permission = z_check_file_permissions;
        }
        code = obj_init(&minst->i_ctx_p, &idmem);  /* requires name_init */
        if (code < 0)
            goto fail;
        minst->init_done = 1;
        code = i_plugin_init(minst->i_ctx_p);
        if (code < 0)
            goto fail;
        code = i_iodev_init(&idmem);
        if (code < 0)
            goto fail;
    }
    return 0;

fail:
    names_free(nt);
    if (minst->i_ctx_p == NULL)
        ialloc_finit(&idmem);
    return code;
}