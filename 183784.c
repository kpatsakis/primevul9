store_config_flag(struct lys_node *node, int options)
{
    switch (node->nodetype) {
    case LYS_CONTAINER:
    case LYS_LEAF:
    case LYS_LEAFLIST:
    case LYS_LIST:
    case LYS_CHOICE:
    case LYS_ANYDATA:
    case LYS_ANYXML:
        if (options & LYS_PARSE_OPT_CFG_IGNORE) {
            node->flags |= node->flags & (~(LYS_CONFIG_MASK | LYS_CONFIG_SET));
        } else if (!(options & LYS_PARSE_OPT_CFG_NOINHERIT)) {
            if (!(node->flags & LYS_CONFIG_MASK)) {
                /* get config flag from parent */
                if (node->parent) {
                    node->flags |= node->parent->flags & LYS_CONFIG_MASK;
                } else {
                    /* default config is true */
                    node->flags |= LYS_CONFIG_W;
                }
            }
        }
        break;
    case LYS_CASE:
        if (!(options & (LYS_PARSE_OPT_CFG_IGNORE | LYS_PARSE_OPT_CFG_NOINHERIT))) {
            if (!(node->flags & LYS_CONFIG_MASK)) {
                /* get config flag from parent */
                if (node->parent) {
                    node->flags |= node->parent->flags & LYS_CONFIG_MASK;
                } else {
                    /* default config is true */
                    node->flags |= LYS_CONFIG_W;
                }
            }
        }
        break;
    default:
        break;
    }

    return EXIT_SUCCESS;
}