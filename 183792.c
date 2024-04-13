yang_read_augment(struct lys_module *module, struct lys_node *parent, struct lys_node_augment *aug, char *value)
{
    aug->nodetype = LYS_AUGMENT;
    aug->target_name = transform_schema2json(module, value);
    free(value);
    if (!aug->target_name) {
        return EXIT_FAILURE;
    }
    aug->parent = parent;
    aug->module = module;
    return EXIT_SUCCESS;
}