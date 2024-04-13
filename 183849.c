yang_read_node(struct lys_module *module, struct lys_node *parent, struct lys_node **root,
               char *value, int nodetype, int sizeof_struct)
{
    struct lys_node *node, **child;

    node = calloc(1, sizeof_struct);
    LY_CHECK_ERR_RETURN(!node, LOGMEM(module->ctx); free(value), NULL);

    LOGDBG(LY_LDGYANG, "parsing %s statement \"%s\"", strnodetype(nodetype), value);
    node->name = lydict_insert_zc(module->ctx, value);
    node->module = module;
    node->nodetype = nodetype;
    node->parent = parent;

    /* insert the node into the schema tree */
    child = (parent) ? &parent->child : root;
    if (*child) {
        (*child)->prev->next = node;
        (*child)->prev = node;
    } else {
        *child = node;
        node->prev = node;
    }
    return node;
}