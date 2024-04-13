lyp_get_yang_data_template_name(const struct lyd_node *node)
{
    struct lys_node *snode;

    snode = lys_parent(node->schema);
    while (snode && snode->nodetype & (LYS_USES | LYS_CASE | LYS_CHOICE)) {
        snode = lys_parent(snode);
    }

    if (snode && snode->nodetype == LYS_EXT && strcmp(((struct lys_ext_instance_complex *)snode)->def->name, "yang-data") == 0) {
        return ((struct lys_ext_instance_complex *)snode)->arg_value;
    } else {
        return NULL;
    }
}