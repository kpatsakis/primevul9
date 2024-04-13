lyp_fill_attr(struct ly_ctx *ctx, struct lyd_node *parent, const char *module_ns, const char *module_name,
              const char *attr_name, const char *attr_value, struct lyxml_elem *xml, int options, struct lyd_attr **ret)
{
    const struct lys_module *mod = NULL;
    const struct lys_submodule *submod = NULL;
    struct lys_type **type;
    struct lyd_attr *dattr;
    int pos, i, j, k;

    /* first, get module where the annotation should be defined */
    if (module_ns) {
        mod = (struct lys_module *)ly_ctx_get_module_by_ns(ctx, module_ns, NULL, 0);
    } else if (module_name) {
        mod = (struct lys_module *)ly_ctx_get_module(ctx, module_name, NULL, 0);
    } else {
        LOGINT(ctx);
        return -1;
    }
    if (!mod) {
        return 1;
    }

    /* then, find the appropriate annotation definition */
    pos = -1;
    for (i = 0, j = 0; i < mod->ext_size; i = i + j + 1) {
        j = lys_ext_instance_presence(&ctx->models.list[0]->extensions[0], &mod->ext[i], mod->ext_size - i);
        if (j == -1) {
            break;
        }
        if (ly_strequal(mod->ext[i + j]->arg_value, attr_name, 0)) {
            pos = i + j;
            break;
        }
    }

    /* try submodules */
    if (pos == -1) {
        for (k = 0; k < mod->inc_size; ++k) {
            submod = mod->inc[k].submodule;
            for (i = 0, j = 0; i < submod->ext_size; i = i + j + 1) {
                j = lys_ext_instance_presence(&ctx->models.list[0]->extensions[0], &submod->ext[i], submod->ext_size - i);
                if (j == -1) {
                    break;
                }
                if (ly_strequal(submod->ext[i + j]->arg_value, attr_name, 0)) {
                    pos = i + j;
                    break;
                }
            }
        }
    }

    if (pos == -1) {
        return 1;
    }

    /* allocate and fill the data attribute structure */
    dattr = calloc(1, sizeof *dattr);
    LY_CHECK_ERR_RETURN(!dattr, LOGMEM(ctx), -1);

    dattr->parent = parent;
    dattr->next = NULL;
    dattr->annotation = submod ? (struct lys_ext_instance_complex *)submod->ext[pos] :
                                 (struct lys_ext_instance_complex *)mod->ext[pos];
    dattr->name = lydict_insert(ctx, attr_name, 0);
    dattr->value_str = lydict_insert(ctx, attr_value, 0);

    /* the value is here converted to a JSON format if needed in case of LY_TYPE_IDENT and LY_TYPE_INST or to a
     * canonical form of the value */
    type = lys_ext_complex_get_substmt(LY_STMT_TYPE, dattr->annotation, NULL);
    if (!type || !lyp_parse_value(*type, &dattr->value_str, xml, NULL, dattr, NULL, 1, 0, options & LYD_OPT_TRUSTED)) {
        lydict_remove(ctx, dattr->name);
        lydict_remove(ctx, dattr->value_str);
        free(dattr);
        return -1;
    }

    *ret = dattr;
    return 0;
}