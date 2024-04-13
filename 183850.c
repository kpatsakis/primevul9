yang_check_ext_instance(struct lys_module *module, struct lys_ext_instance ***ext, uint size,
                        void *parent, struct unres_schema *unres)
{
    struct unres_ext *info;
    uint i;

    for (i = 0; i < size; ++i) {
        info = malloc(sizeof *info);
        LY_CHECK_ERR_RETURN(!info, LOGMEM(module->ctx), EXIT_FAILURE);
        info->data.yang = (*ext)[i]->parent;
        info->datatype = LYS_IN_YANG;
        info->parent = parent;
        info->mod = module;
        info->parent_type = (*ext)[i]->parent_type;
        info->substmt = (*ext)[i]->insubstmt;
        info->substmt_index = (*ext)[i]->insubstmt_index;
        info->ext_index = i;
        if (unres_schema_add_node(module, unres, ext, UNRES_EXT, (struct lys_node *)info) == -1) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}