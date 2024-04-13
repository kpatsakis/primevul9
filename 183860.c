yang_check_rpc_action(struct lys_module *module, struct lys_node_rpc_action *rpc, struct lys_node **child,
                      int options, struct unres_schema *unres)
{
    struct lys_node *node;

    if (rpc->nodetype == LYS_ACTION) {
        for (node = rpc->parent; node; node = lys_parent(node)) {
            if ((node->nodetype & (LYS_RPC | LYS_ACTION | LYS_NOTIF))
                    || ((node->nodetype == LYS_LIST) && !((struct lys_node_list *)node)->keys)) {
                LOGVAL(module->ctx, LYE_INPAR, LY_VLOG_LYS, rpc->parent, strnodetype(node->nodetype), "action");
                goto error;
            }
        }
    }
    if (yang_check_typedef(module, (struct lys_node *)rpc, unres)) {
        goto error;
    }

    if (yang_check_iffeatures(module, NULL, rpc, RPC_KEYWORD, unres)) {
        goto error;
    }

    if (yang_check_nodes(module, (struct lys_node *)rpc, *child, options | LYS_PARSE_OPT_CFG_IGNORE, unres)) {
        *child = NULL;
        goto error;
    }
    *child = NULL;

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}