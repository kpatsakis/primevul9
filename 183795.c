yang_free_nodes(struct ly_ctx *ctx, struct lys_node *node)
{
    struct lys_node *tmp, *child, *sibling;

    if (!node) {
        return;
    }
    tmp = node;

    while (tmp) {
        child = tmp->child;
        sibling = tmp->next;
        /* common part */
        lydict_remove(ctx, tmp->name);
        if (!(tmp->nodetype & (LYS_INPUT | LYS_OUTPUT))) {
            lys_iffeature_free(ctx, tmp->iffeature, tmp->iffeature_size, 0, NULL);
            lydict_remove(ctx, tmp->dsc);
            lydict_remove(ctx, tmp->ref);
        }

        switch (tmp->nodetype) {
        case LYS_GROUPING:
        case LYS_RPC:
        case LYS_ACTION:
            yang_free_grouping(ctx, (struct lys_node_grp *)tmp);
            break;
        case LYS_CONTAINER:
            yang_free_container(ctx, (struct lys_node_container *)tmp);
            break;
        case LYS_LEAF:
            yang_free_leaf(ctx, (struct lys_node_leaf *)tmp);
            break;
        case LYS_LEAFLIST:
            yang_free_leaflist(ctx, (struct lys_node_leaflist *)tmp);
            break;
        case LYS_LIST:
            yang_free_list(ctx, (struct lys_node_list *)tmp);
            break;
        case LYS_CHOICE:
            yang_free_choice(ctx, (struct lys_node_choice *)tmp);
            break;
        case LYS_CASE:
            lys_when_free(ctx, ((struct lys_node_case *)tmp)->when, NULL);
            break;
        case LYS_ANYXML:
        case LYS_ANYDATA:
            yang_free_anydata(ctx, (struct lys_node_anydata *)tmp);
            break;
        case LYS_INPUT:
        case LYS_OUTPUT:
            yang_free_inout(ctx, (struct lys_node_inout *)tmp);
            break;
        case LYS_NOTIF:
            yang_free_notif(ctx, (struct lys_node_notif *)tmp);
            break;
        case LYS_USES:
            yang_free_uses(ctx, (struct lys_node_uses *)tmp);
            break;
        default:
            break;
        }
        lys_extension_instances_free(ctx, tmp->ext, tmp->ext_size, NULL);
        yang_free_nodes(ctx, child);
        free(tmp);
        tmp = sibling;
    }
}