lys_ext_dup(struct ly_ctx *ctx, struct lys_module *mod, struct lys_ext_instance **orig, uint8_t size, void *parent,
            LYEXT_PAR parent_type, struct lys_ext_instance ***new, int shallow, struct unres_schema *unres)
{
    int i;
    uint8_t u = 0;
    struct lys_ext_instance **result;
    struct unres_ext *info, *info_orig;
    size_t len;

    assert(new);

    if (!size) {
        if (orig) {
            LOGINT(ctx);
            return EXIT_FAILURE;
        }
        (*new) = NULL;
        return EXIT_SUCCESS;
    }

    (*new) = result = calloc(size, sizeof *result);
    LY_CHECK_ERR_RETURN(!result, LOGMEM(ctx), EXIT_FAILURE);
    for (u = 0; u < size; u++) {
        if (orig[u]) {
            /* resolved extension instance, just duplicate it */
            switch(orig[u]->ext_type) {
            case LYEXT_FLAG:
                result[u] = malloc(sizeof(struct lys_ext_instance));
                LY_CHECK_ERR_GOTO(!result[u], LOGMEM(ctx), error);
                break;
            case LYEXT_COMPLEX:
                len = ((struct lyext_plugin_complex*)orig[u]->def->plugin)->instance_size;
                result[u] = calloc(1, len);
                LY_CHECK_ERR_GOTO(!result[u], LOGMEM(ctx), error);

                ((struct lys_ext_instance_complex*)result[u])->substmt = ((struct lyext_plugin_complex*)orig[u]->def->plugin)->substmt;
                /* TODO duplicate data in extension instance content */
                memcpy((void*)result[u] + sizeof(**orig), (void*)orig[u] + sizeof(**orig), len - sizeof(**orig));
                break;
            }
            /* generic part */
            result[u]->def = orig[u]->def;
            result[u]->flags = LYEXT_OPT_CONTENT;
            result[u]->arg_value = lydict_insert(ctx, orig[u]->arg_value, 0);
            result[u]->parent = parent;
            result[u]->parent_type = parent_type;
            result[u]->insubstmt = orig[u]->insubstmt;
            result[u]->insubstmt_index = orig[u]->insubstmt_index;
            result[u]->ext_type = orig[u]->ext_type;
            result[u]->priv = NULL;
            result[u]->nodetype = LYS_EXT;
            result[u]->module = mod;

            /* extensions */
            result[u]->ext_size = orig[u]->ext_size;
            if (lys_ext_dup(ctx, mod, orig[u]->ext, orig[u]->ext_size, result[u],
                            LYEXT_PAR_EXTINST, &result[u]->ext, shallow, unres)) {
                goto error;
            }

            /* in case of shallow copy (duplication for deviation), duplicate only the link to private data
             * in a new copy, otherwise (grouping instantiation) do not duplicate the private data */
            if (shallow) {
                result[u]->priv = orig[u]->priv;
            }
        } else {
            /* original extension is not yet resolved, so duplicate it in unres */
            i = unres_schema_find(unres, -1, &orig, UNRES_EXT);
            if (i == -1) {
                /* extension not found in unres */
                LOGINT(ctx);
                goto error;
            }
            info_orig = unres->str_snode[i];
            info = malloc(sizeof *info);
            LY_CHECK_ERR_GOTO(!info, LOGMEM(ctx), error);
            info->datatype = info_orig->datatype;
            if (info->datatype == LYS_IN_YIN) {
                info->data.yin = lyxml_dup_elem(ctx, info_orig->data.yin, NULL, 1, 0);
            } /* else TODO YANG */
            info->parent = parent;
            info->mod = mod;
            info->parent_type = parent_type;
            info->ext_index = u;
            if (unres_schema_add_node(info->mod, unres, new, UNRES_EXT, (struct lys_node *)info) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    (*new) = NULL;
    lys_extension_instances_free(ctx, result, u, NULL);
    return EXIT_FAILURE;
}