lyp_check_identifier(struct ly_ctx *ctx, const char *id, enum LY_IDENT type, struct lys_module *module,
                     struct lys_node *parent)
{
    int i, j;
    int size;
    struct lys_tpdf *tpdf;
    struct lys_node *node;
    struct lys_module *mainmod;
    struct lys_submodule *submod;

    assert(ctx && id);

    /* check id syntax */
    if (!(id[0] >= 'A' && id[0] <= 'Z') && !(id[0] >= 'a' && id[0] <= 'z') && id[0] != '_') {
        LOGVAL(ctx, LYE_INID, LY_VLOG_NONE, NULL, id, "invalid start character");
        return EXIT_FAILURE;
    }
    for (i = 1; id[i]; i++) {
        if (!(id[i] >= 'A' && id[i] <= 'Z') && !(id[i] >= 'a' && id[i] <= 'z')
                && !(id[i] >= '0' && id[i] <= '9') && id[i] != '_' && id[i] != '-' && id[i] != '.') {
            LOGVAL(ctx, LYE_INID, LY_VLOG_NONE, NULL, id, "invalid character");
            return EXIT_FAILURE;
        }
    }

    if (i > 64) {
        LOGWRN(ctx, "Identifier \"%s\" is long, you should use something shorter.", id);
    }

    switch (type) {
    case LY_IDENT_NAME:
        /* check uniqueness of the node within its siblings */
        if (!parent) {
            break;
        }

        LY_TREE_FOR(parent->child, node) {
            if (ly_strequal(node->name, id, 1)) {
                LOGVAL(ctx, LYE_INID, LY_VLOG_NONE, NULL, id, "name duplication");
                return EXIT_FAILURE;
            }
        }
        break;
    case LY_IDENT_TYPE:
        assert(module);
        mainmod = lys_main_module(module);

        /* check collision with the built-in types */
        if (!strcmp(id, "binary") || !strcmp(id, "bits") ||
                !strcmp(id, "boolean") || !strcmp(id, "decimal64") ||
                !strcmp(id, "empty") || !strcmp(id, "enumeration") ||
                !strcmp(id, "identityref") || !strcmp(id, "instance-identifier") ||
                !strcmp(id, "int8") || !strcmp(id, "int16") ||
                !strcmp(id, "int32") || !strcmp(id, "int64") ||
                !strcmp(id, "leafref") || !strcmp(id, "string") ||
                !strcmp(id, "uint8") || !strcmp(id, "uint16") ||
                !strcmp(id, "uint32") || !strcmp(id, "uint64") || !strcmp(id, "union")) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, id, "typedef");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Typedef name duplicates a built-in type.");
            return EXIT_FAILURE;
        }

        /* check locally scoped typedefs (avoid name shadowing) */
        for (; parent; parent = lys_parent(parent)) {
            switch (parent->nodetype) {
            case LYS_CONTAINER:
                size = ((struct lys_node_container *)parent)->tpdf_size;
                tpdf = ((struct lys_node_container *)parent)->tpdf;
                break;
            case LYS_LIST:
                size = ((struct lys_node_list *)parent)->tpdf_size;
                tpdf = ((struct lys_node_list *)parent)->tpdf;
                break;
            case LYS_GROUPING:
                size = ((struct lys_node_grp *)parent)->tpdf_size;
                tpdf = ((struct lys_node_grp *)parent)->tpdf;
                break;
            default:
                continue;
            }

            if (dup_typedef_check(id, tpdf, size)) {
                LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "typedef", id);
                return EXIT_FAILURE;
            }
        }

        /* check top-level names */
        if (dup_typedef_check(id, module->tpdf, module->tpdf_size)) {
            LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "typedef", id);
            return EXIT_FAILURE;
        }

        /* check submodule's top-level names */
        for (i = 0; i < mainmod->inc_size && mainmod->inc[i].submodule; i++) {
            if (dup_typedef_check(id, mainmod->inc[i].submodule->tpdf, mainmod->inc[i].submodule->tpdf_size)) {
                LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "typedef", id);
                return EXIT_FAILURE;
            }
        }

        break;
    case LY_IDENT_PREFIX:
        assert(module);

        /* check the module itself */
        if (dup_prefix_check(id, module)) {
            LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "prefix", id);
            return EXIT_FAILURE;
        }
        break;
    case LY_IDENT_FEATURE:
        assert(module);
        mainmod = lys_main_module(module);

        /* check feature name uniqueness*/
        /* check features in the current module */
        if (dup_feature_check(id, module)) {
            LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "feature", id);
            return EXIT_FAILURE;
        }

        /* and all its submodules */
        for (i = 0; i < mainmod->inc_size && mainmod->inc[i].submodule; i++) {
            if (dup_feature_check(id, (struct lys_module *)mainmod->inc[i].submodule)) {
                LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "feature", id);
                return EXIT_FAILURE;
            }
        }
        break;

    case LY_IDENT_EXTENSION:
        assert(module);
        mainmod = lys_main_module(module);

        /* check extension name uniqueness in the main module ... */
        for (i = 0; i < mainmod->extensions_size; i++) {
            if (ly_strequal(id, mainmod->extensions[i].name, 1)) {
                LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "extension", id);
                return EXIT_FAILURE;
            }
        }

        /* ... and all its submodules */
        for (j = 0; j < mainmod->inc_size && mainmod->inc[j].submodule; j++) {
            submod = mainmod->inc[j].submodule; /* shortcut */
            for (i = 0; i < submod->extensions_size; i++) {
                if (ly_strequal(id, submod->extensions[i].name, 1)) {
                    LOGVAL(ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "extension", id);
                    return EXIT_FAILURE;
                }
            }
        }

        break;

    default:
        /* no check required */
        break;
    }

    return EXIT_SUCCESS;
}