lyp_add_ietf_netconf_annotations_config(struct lys_module *mod)
{
    void *reallocated;
    struct lys_ext_instance_complex *op;
    struct lys_type **type;
    struct lys_node_anydata *anyxml;
    int i;
    struct ly_ctx *ctx = mod->ctx; /* shortcut */

    reallocated = realloc(mod->ext, (mod->ext_size + 3) * sizeof *mod->ext);
    LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(ctx), EXIT_FAILURE);
    mod->ext = reallocated;
    /* 1) edit-config's operation */
    op = calloc(1, (sizeof(struct lys_ext_instance_complex) - 1) + 5 * sizeof(void*) + sizeof(uint16_t));
    LY_CHECK_ERR_RETURN(!op, LOGMEM(ctx), EXIT_FAILURE);
    mod->ext[mod->ext_size] = (struct lys_ext_instance *)op;
    op->arg_value = lydict_insert(ctx, "operation", 9);
    op->def = &ctx->models.list[0]->extensions[0];
    op->ext_type = LYEXT_COMPLEX;
    op->module = op->parent = mod;
    op->parent_type = LYEXT_PAR_MODULE;
    op->substmt = ((struct lyext_plugin_complex *)op->def->plugin)->substmt;
    op->nodetype = LYS_EXT;
    type = (struct lys_type**)&op->content; /* type is stored at offset 0 */
    *type = calloc(1, sizeof(struct lys_type));
    LY_CHECK_ERR_RETURN(!*type, LOGMEM(ctx), EXIT_FAILURE);
    (*type)->base = LY_TYPE_ENUM;
    (*type)->der = ly_types[LY_TYPE_ENUM];
    (*type)->parent = (struct lys_tpdf *)op;
    (*type)->info.enums.count = 5;
    (*type)->info.enums.enm = calloc(5, sizeof *(*type)->info.enums.enm);
    LY_CHECK_ERR_RETURN(!(*type)->info.enums.enm, LOGMEM(ctx), EXIT_FAILURE);
    (*type)->info.enums.enm[0].value = 0;
    (*type)->info.enums.enm[0].name = lydict_insert(ctx, "merge", 5);
    (*type)->info.enums.enm[1].value = 1;
    (*type)->info.enums.enm[1].name = lydict_insert(ctx, "replace", 7);
    (*type)->info.enums.enm[2].value = 2;
    (*type)->info.enums.enm[2].name = lydict_insert(ctx, "create", 6);
    (*type)->info.enums.enm[3].value = 3;
    (*type)->info.enums.enm[3].name = lydict_insert(ctx, "delete", 6);
    (*type)->info.enums.enm[4].value = 4;
    (*type)->info.enums.enm[4].name = lydict_insert(ctx, "remove", 6);
    mod->ext_size++;

    /* 2) filter's type */
    op = calloc(1, (sizeof(struct lys_ext_instance_complex) - 1) + 5 * sizeof(void*) + sizeof(uint16_t));
    LY_CHECK_ERR_RETURN(!op, LOGMEM(ctx), EXIT_FAILURE);
    mod->ext[mod->ext_size] = (struct lys_ext_instance *)op;
    op->arg_value = lydict_insert(ctx, "type", 4);
    op->def = &ctx->models.list[0]->extensions[0];
    op->ext_type = LYEXT_COMPLEX;
    op->module = op->parent = mod;
    op->parent_type = LYEXT_PAR_MODULE;
    op->substmt = ((struct lyext_plugin_complex *)op->def->plugin)->substmt;
    op->nodetype = LYS_EXT;
    type = (struct lys_type**)&op->content; /* type is stored at offset 0 */
    *type = calloc(1, sizeof(struct lys_type));
    LY_CHECK_ERR_RETURN(!*type, LOGMEM(ctx), EXIT_FAILURE);
    (*type)->base = LY_TYPE_ENUM;
    (*type)->der = ly_types[LY_TYPE_ENUM];
    (*type)->parent = (struct lys_tpdf *)op;
    (*type)->info.enums.count = 2;
    (*type)->info.enums.enm = calloc(2, sizeof *(*type)->info.enums.enm);
    LY_CHECK_ERR_RETURN(!(*type)->info.enums.enm, LOGMEM(ctx), EXIT_FAILURE);
    (*type)->info.enums.enm[0].value = 0;
    (*type)->info.enums.enm[0].name = lydict_insert(ctx, "subtree", 7);
    (*type)->info.enums.enm[1].value = 1;
    (*type)->info.enums.enm[1].name = lydict_insert(ctx, "xpath", 5);
    for (i = mod->features_size; i > 0; i--) {
        if (!strcmp(mod->features[i - 1].name, "xpath")) {
            (*type)->info.enums.enm[1].iffeature_size = 1;
            (*type)->info.enums.enm[1].iffeature = calloc(1, sizeof(struct lys_feature));
            LY_CHECK_ERR_RETURN(!(*type)->info.enums.enm[1].iffeature, LOGMEM(ctx), EXIT_FAILURE);
            (*type)->info.enums.enm[1].iffeature[0].expr = malloc(sizeof(uint8_t));
            LY_CHECK_ERR_RETURN(!(*type)->info.enums.enm[1].iffeature[0].expr, LOGMEM(ctx), EXIT_FAILURE);
            *(*type)->info.enums.enm[1].iffeature[0].expr = 3; /* LYS_IFF_F */
            (*type)->info.enums.enm[1].iffeature[0].features = malloc(sizeof(struct lys_feature*));
            LY_CHECK_ERR_RETURN(!(*type)->info.enums.enm[1].iffeature[0].features, LOGMEM(ctx), EXIT_FAILURE);
            (*type)->info.enums.enm[1].iffeature[0].features[0] = &mod->features[i - 1];
            break;
        }
    }
    mod->ext_size++;

    /* 3) filter's select */
    op = calloc(1, (sizeof(struct lys_ext_instance_complex) - 1) + 5 * sizeof(void*) + sizeof(uint16_t));
    LY_CHECK_ERR_RETURN(!op, LOGMEM(ctx), EXIT_FAILURE);
    mod->ext[mod->ext_size] = (struct lys_ext_instance *)op;
    op->arg_value = lydict_insert(ctx, "select", 6);
    op->def = &ctx->models.list[0]->extensions[0];
    op->ext_type = LYEXT_COMPLEX;
    op->module = op->parent = mod;
    op->parent_type = LYEXT_PAR_MODULE;
    op->substmt = ((struct lyext_plugin_complex *)op->def->plugin)->substmt;
    op->nodetype = LYS_EXT;
    type = (struct lys_type**)&op->content; /* type is stored at offset 0 */
    *type = calloc(1, sizeof(struct lys_type));
    LY_CHECK_ERR_RETURN(!*type, LOGMEM(ctx), EXIT_FAILURE);
    (*type)->base = LY_TYPE_STRING;
    (*type)->der = ly_types[LY_TYPE_STRING];
    (*type)->parent = (struct lys_tpdf *)op;
    mod->ext_size++;

    /* 4) URL config */
    anyxml = calloc(1, sizeof *anyxml);
    LY_CHECK_ERR_RETURN(!anyxml, LOGMEM(ctx), EXIT_FAILURE);
    anyxml->nodetype = LYS_ANYXML;
    anyxml->prev = (struct lys_node *)anyxml;
    anyxml->name = lydict_insert(ctx, "config", 0);
    anyxml->module = mod;
    anyxml->flags = LYS_CONFIG_W;
    if (lys_node_addchild(NULL, mod, (struct lys_node *)anyxml, 0)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}