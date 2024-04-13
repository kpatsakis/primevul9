lyp_is_rpc_action(struct lys_node *node)
{
    assert(node);

    while (lys_parent(node)) {
        node = lys_parent(node);
        if (node->nodetype == LYS_ACTION) {
            break;
        }
    }

    if (node->nodetype & (LYS_RPC | LYS_ACTION)) {
        return 1;
    } else {
        return 0;
    }
}