resolve_partial_json_data_list_predicate(struct parsed_pred pp, struct lyd_node *node, int position)
{
    uint16_t i;
    char *val_str;
    struct lyd_node_leaf_list *key;
    struct lys_node_list *slist;
    struct ly_ctx *ctx;

    assert(node);
    assert(node->schema->nodetype == LYS_LIST);
    assert(pp.len);

    ctx = node->schema->module->ctx;
    slist = (struct lys_node_list *)node->schema;

    /* is the predicate a number? */
    if (isdigit(pp.pred[0].name[0])) {
        if (position == atoi(pp.pred[0].name)) {
            /* match */
            return 0;
        } else {
            /* not a match */
            return 1;
        }
    }

    key = (struct lyd_node_leaf_list *)node->child;
    if (!key) {
        /* it is not a position, so we need a key for it to be a match */
        return 1;
    }

    /* go through all the keys */
    for (i = 0; i < slist->keys_size; ++i) {
        if (strncmp(key->schema->name, pp.pred[i].name, pp.pred[i].nam_len) || key->schema->name[pp.pred[i].nam_len]) {
            LOGVAL(ctx, LYE_PATH_INKEY, LY_VLOG_NONE, NULL, pp.pred[i].name);
            return -1;
        }

        if (pp.pred[i].mod_name) {
            /* specific module, check that the found key is from that module */
            if (strncmp(lyd_node_module((struct lyd_node *)key)->name, pp.pred[i].mod_name, pp.pred[i].mod_name_len)
                    || lyd_node_module((struct lyd_node *)key)->name[pp.pred[i].mod_name_len]) {
                LOGVAL(ctx, LYE_PATH_INKEY, LY_VLOG_NONE, NULL, pp.pred[i].name);
                return -1;
            }

            /* but if the module is the same as the parent, it should have been omitted */
            if (lyd_node_module((struct lyd_node *)key) == lyd_node_module(node)) {
                LOGVAL(ctx, LYE_PATH_INKEY, LY_VLOG_NONE, NULL, pp.pred[i].name);
                return -1;
            }
        } else {
            /* no module, so it must be the same as the list (parent) */
            if (lyd_node_module((struct lyd_node *)key) != lyd_node_module(node)) {
                LOGVAL(ctx, LYE_PATH_INKEY, LY_VLOG_NONE, NULL, pp.pred[i].name);
                return -1;
            }
        }

        /* get canonical value */
        val_str = lyd_make_canonical(key->schema, pp.pred[i].value, pp.pred[i].val_len);
        if (!val_str) {
            return -1;
        }

        /* value does not match */
        if (strcmp(key->value_str, val_str)) {
            free(val_str);
            return 1;
        }
        free(val_str);

        key = (struct lyd_node_leaf_list *)key->next;
    }

    return 0;
}