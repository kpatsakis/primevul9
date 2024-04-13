yang_fill_iffeature(struct lys_module *module, struct lys_iffeature *iffeature, void *parent,
                    char *value, struct unres_schema *unres, int parent_is_feature)
{
    const char *exp;
    int ret;

    if ((module->version != 2) && ((value[0] == '(') || strchr(value, ' '))) {
        LOGVAL(module->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "if-feature");
        free(value);
        return EXIT_FAILURE;
    }

    if (!(exp = transform_iffeat_schema2json(module, value))) {
        free(value);
        return EXIT_FAILURE;
    }
    free(value);

    ret = resolve_iffeature_compile(iffeature, exp, (struct lys_node *)parent, parent_is_feature, unres);
    lydict_remove(module->ctx, exp);

    return (ret) ? EXIT_FAILURE : EXIT_SUCCESS;
}