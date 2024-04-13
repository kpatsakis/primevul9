ident_val_add_module_prefix(const char *value, const struct lyxml_elem *xml, struct ly_ctx *ctx)
{
    const struct lyxml_ns *ns;
    const struct lys_module *mod;
    char *str;

    do {
        LY_TREE_FOR((struct lyxml_ns *)xml->attr, ns) {
            if ((ns->type == LYXML_ATTR_NS) && !ns->prefix) {
                /* match */
                break;
            }
        }
        if (!ns) {
            xml = xml->parent;
        }
    } while (!ns && xml);

    if (!ns) {
        /* no default namespace */
        LOGINT(ctx);
        return NULL;
    }

    /* find module */
    mod = ly_ctx_get_module_by_ns(ctx, ns->value, NULL, 1);
    if (!mod) {
        LOGINT(ctx);
        return NULL;
    }

    if (asprintf(&str, "%s:%s", mod->name, value) == -1) {
        LOGMEM(ctx);
        return NULL;
    }
    lydict_remove(ctx, value);

    return lydict_insert_zc(ctx, str);
}