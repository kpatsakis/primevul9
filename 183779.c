yang_read_base(struct lys_module *module, struct lys_ident *ident, char *value, struct unres_schema *unres)
{
    const char *exp;

    exp = transform_schema2json(module, value);
    free(value);
    if (!exp) {
        return EXIT_FAILURE;
    }

    if (unres_schema_add_str(module, unres, ident, UNRES_IDENT, exp) == -1) {
        lydict_remove(module->ctx, exp);
        return EXIT_FAILURE;
    }

    lydict_remove(module->ctx, exp);
    return EXIT_SUCCESS;
}