resolve_extension(struct unres_ext *info, struct lys_ext_instance **ext, struct unres_schema *unres)
{
    enum LY_VLOG_ELEM vlog_type;
    void *vlog_node;
    unsigned int i, j;
    struct lys_ext *e;
    char *ext_name, *ext_prefix, *tmp;
    struct lyxml_elem *next_yin, *yin;
    const struct lys_module *mod;
    struct lys_ext_instance *tmp_ext;
    struct ly_ctx *ctx = NULL;
    LYEXT_TYPE etype;

    switch (info->parent_type) {
    case LYEXT_PAR_NODE:
        vlog_node = info->parent;
        vlog_type = LY_VLOG_LYS;
        break;
    case LYEXT_PAR_MODULE:
    case LYEXT_PAR_IMPORT:
    case LYEXT_PAR_INCLUDE:
        vlog_node = NULL;
        vlog_type = LY_VLOG_LYS;
        break;
    default:
        vlog_node = NULL;
        vlog_type = LY_VLOG_NONE;
        break;
    }

    if (info->datatype == LYS_IN_YIN) {
        /* YIN */

        /* get the module where the extension is supposed to be defined */
        mod = lyp_get_import_module_ns(info->mod, info->data.yin->ns->value);
        if (!mod) {
            LOGVAL(ctx, LYE_INSTMT, vlog_type, vlog_node, info->data.yin->name);
            return EXIT_FAILURE;
        }
        ctx = mod->ctx;

        /* find the extension definition */
        e = NULL;
        for (i = 0; i < mod->extensions_size; i++) {
            if (ly_strequal(mod->extensions[i].name, info->data.yin->name, 1)) {
                e = &mod->extensions[i];
                break;
            }
        }
        /* try submodules */
        for (j = 0; !e && j < mod->inc_size; j++) {
            for (i = 0; i < mod->inc[j].submodule->extensions_size; i++) {
                if (ly_strequal(mod->inc[j].submodule->extensions[i].name, info->data.yin->name, 1)) {
                    e = &mod->inc[j].submodule->extensions[i];
                    break;
                }
            }
        }
        if (!e) {
            LOGVAL(ctx, LYE_INSTMT, vlog_type, vlog_node, info->data.yin->name);
            return EXIT_FAILURE;
        }

        /* we have the extension definition, so now it cannot be forward referenced and error is always fatal */

        if (e->plugin && e->plugin->check_position) {
            /* common part - we have plugin with position checking function, use it first */
            if ((*e->plugin->check_position)(info->parent, info->parent_type, info->substmt)) {
                /* extension is not allowed here */
                LOGVAL(ctx, LYE_INSTMT, vlog_type, vlog_node, e->name);
                return -1;
            }
        }

        /* extension type-specific part - allocation */
        if (e->plugin) {
            etype = e->plugin->type;
        } else {
            /* default type */
            etype = LYEXT_FLAG;
        }
        switch (etype) {
        case LYEXT_FLAG:
            (*ext) = calloc(1, sizeof(struct lys_ext_instance));
            break;
        case LYEXT_COMPLEX:
            (*ext) = calloc(1, ((struct lyext_plugin_complex*)e->plugin)->instance_size);
            break;
        case LYEXT_ERR:
            /* we never should be here */
            LOGINT(ctx);
            return -1;
        }
        LY_CHECK_ERR_RETURN(!*ext, LOGMEM(ctx), -1);

        /* common part for all extension types */
        (*ext)->def = e;
        (*ext)->parent = info->parent;
        (*ext)->parent_type = info->parent_type;
        (*ext)->insubstmt = info->substmt;
        (*ext)->insubstmt_index = info->substmt_index;
        (*ext)->ext_type = e->plugin ? e->plugin->type : LYEXT_FLAG;
        (*ext)->flags |= e->plugin ? e->plugin->flags : 0;

        if (e->argument) {
            if (!(e->flags & LYS_YINELEM)) {
                (*ext)->arg_value = lyxml_get_attr(info->data.yin, e->argument, NULL);
                if (!(*ext)->arg_value) {
                    LOGVAL(ctx, LYE_MISSARG, LY_VLOG_NONE, NULL, e->argument, info->data.yin->name);
                    return -1;
                }

                (*ext)->arg_value = lydict_insert(mod->ctx, (*ext)->arg_value, 0);
            } else {
                LY_TREE_FOR_SAFE(info->data.yin->child, next_yin, yin) {
                    if (ly_strequal(yin->name, e->argument, 1)) {
                        (*ext)->arg_value = lydict_insert(mod->ctx, yin->content, 0);
                        lyxml_free(mod->ctx, yin);
                        break;
                    }
                }
            }
        }

        if ((*ext)->flags & LYEXT_OPT_VALID &&
            (info->parent_type == LYEXT_PAR_NODE || info->parent_type == LYEXT_PAR_TPDF)) {
            ((struct lys_node *)info->parent)->flags |= LYS_VALID_EXT;
        }

        (*ext)->nodetype = LYS_EXT;
        (*ext)->module = info->mod;

        /* extension type-specific part - parsing content */
        switch (etype) {
        case LYEXT_FLAG:
            LY_TREE_FOR_SAFE(info->data.yin->child, next_yin, yin) {
                if (!yin->ns) {
                    /* garbage */
                    lyxml_free(mod->ctx, yin);
                    continue;
                } else if (!strcmp(yin->ns->value, LY_NSYIN)) {
                    /* standard YANG statements are not expected here */
                    LOGVAL(ctx, LYE_INCHILDSTMT, vlog_type, vlog_node, yin->name, info->data.yin->name);
                    return -1;
                } else if (yin->ns == info->data.yin->ns &&
                        (e->flags & LYS_YINELEM) && ly_strequal(yin->name, e->argument, 1)) {
                    /* we have the extension's argument */
                    if ((*ext)->arg_value) {
                        LOGVAL(ctx, LYE_TOOMANY, vlog_type, vlog_node, yin->name, info->data.yin->name);
                        return -1;
                    }
                    (*ext)->arg_value = yin->content;
                    yin->content = NULL;
                    lyxml_free(mod->ctx, yin);
                } else {
                    /* extension instance */
                    if (lyp_yin_parse_subnode_ext(info->mod, *ext, LYEXT_PAR_EXTINST, yin,
                                                  LYEXT_SUBSTMT_SELF, 0, unres)) {
                        return -1;
                    }

                    continue;
                }
            }
            break;
        case LYEXT_COMPLEX:
            ((struct lys_ext_instance_complex*)(*ext))->substmt = ((struct lyext_plugin_complex*)e->plugin)->substmt;
            if (lyp_yin_parse_complex_ext(info->mod, (struct lys_ext_instance_complex*)(*ext), info->data.yin, unres)) {
                /* TODO memory cleanup */
                return -1;
            }
            break;
        default:
            break;
        }

        /* TODO - lyext_check_result_clb, other than LYEXT_FLAG plugins */

    } else {
        /* YANG */

        ext_prefix = (char *)(*ext)->def;
        tmp = strchr(ext_prefix, ':');
        if (!tmp) {
            LOGVAL(ctx, LYE_INSTMT, vlog_type, vlog_node, ext_prefix);
            goto error;
        }
        ext_name = tmp + 1;

        /* get the module where the extension is supposed to be defined */
        mod = lyp_get_module(info->mod, ext_prefix, tmp - ext_prefix, NULL, 0, 0);
        if (!mod) {
            LOGVAL(ctx, LYE_INSTMT, vlog_type, vlog_node, ext_prefix);
            return EXIT_FAILURE;
        }
        ctx = mod->ctx;

        /* find the extension definition */
        e = NULL;
        for (i = 0; i < mod->extensions_size; i++) {
            if (ly_strequal(mod->extensions[i].name, ext_name, 0)) {
                e = &mod->extensions[i];
                break;
            }
        }
        /* try submodules */
        for (j = 0; !e && j < mod->inc_size; j++) {
            for (i = 0; i < mod->inc[j].submodule->extensions_size; i++) {
                if (ly_strequal(mod->inc[j].submodule->extensions[i].name, ext_name, 0)) {
                    e = &mod->inc[j].submodule->extensions[i];
                    break;
                }
            }
        }
        if (!e) {
            LOGVAL(ctx, LYE_INSTMT, vlog_type, vlog_node, ext_prefix);
            return EXIT_FAILURE;
        }

        (*ext)->flags &= ~LYEXT_OPT_YANG;
        (*ext)->def = NULL;

        /* we have the extension definition, so now it cannot be forward referenced and error is always fatal */

        if (e->plugin && e->plugin->check_position) {
            /* common part - we have plugin with position checking function, use it first */
            if ((*e->plugin->check_position)(info->parent, info->parent_type, info->substmt)) {
                /* extension is not allowed here */
                LOGVAL(ctx, LYE_INSTMT, vlog_type, vlog_node, e->name);
                goto error;
            }
        }

        if (e->argument && !(*ext)->arg_value) {
            LOGVAL(ctx, LYE_MISSARG, LY_VLOG_NONE, NULL, e->argument, ext_name);
            goto error;
        }

        /* extension common part */
        (*ext)->def = e;
        (*ext)->parent = info->parent;
        (*ext)->ext_type = e->plugin ? e->plugin->type : LYEXT_FLAG;
        (*ext)->flags |= e->plugin ? e->plugin->flags : 0;

        if ((*ext)->flags & LYEXT_OPT_VALID &&
            (info->parent_type == LYEXT_PAR_NODE || info->parent_type == LYEXT_PAR_TPDF)) {
            ((struct lys_node *)info->parent)->flags |= LYS_VALID_EXT;
        }

        (*ext)->module = info->mod;
        (*ext)->nodetype = LYS_EXT;

        /* extension type-specific part */
        if (e->plugin) {
            etype = e->plugin->type;
        } else {
            /* default type */
            etype = LYEXT_FLAG;
        }
        switch (etype) {
        case LYEXT_FLAG:
            /* nothing change */
            break;
        case LYEXT_COMPLEX:
            tmp_ext = realloc(*ext, ((struct lyext_plugin_complex*)e->plugin)->instance_size);
            LY_CHECK_ERR_GOTO(!tmp_ext, LOGMEM(ctx), error);
            memset((char *)tmp_ext + offsetof(struct lys_ext_instance_complex, content), 0,
                   ((struct lyext_plugin_complex*)e->plugin)->instance_size - offsetof(struct lys_ext_instance_complex, content));
            (*ext) = tmp_ext;
            ((struct lys_ext_instance_complex*)(*ext))->substmt = ((struct lyext_plugin_complex*)e->plugin)->substmt;
            if (info->data.yang) {
                *tmp = ':';
                if (yang_parse_ext_substatement(info->mod, unres, info->data.yang->ext_substmt, ext_prefix,
                                                (struct lys_ext_instance_complex*)(*ext))) {
                    goto error;
                }
                if (yang_fill_extcomplex_module(info->mod->ctx, (struct lys_ext_instance_complex*)(*ext), ext_prefix,
                                                info->data.yang->ext_modules, info->mod->implemented)) {
                    goto error;
                }
            }
            if (lyp_mand_check_ext((struct lys_ext_instance_complex*)(*ext), ext_prefix)) {
                goto error;
            }
            break;
        case LYEXT_ERR:
            /* we never should be here */
            LOGINT(ctx);
            goto error;
        }

        if (yang_check_ext_instance(info->mod, &(*ext)->ext, (*ext)->ext_size, *ext, unres)) {
            goto error;
        }
        free(ext_prefix);
    }

    return EXIT_SUCCESS;
error:
    free(ext_prefix);
    return -1;
}