yang_check_identityref(struct lys_module *module, struct lys_type *type, struct unres_schema *unres)
{
    uint size, i;
    int rc;
    struct lys_ident **ref;
    const char *value;
    char *expr;

    ref = type->info.ident.ref;
    size = type->info.ident.count;
    type->info.ident.count = 0;
    type->info.ident.ref = NULL;
    ((struct yang_type *)type->der)->flags |= LYS_NO_ERASE_IDENTITY;

    for (i = 0; i < size; ++i) {
        expr = (char *)ref[i];
        /* store in the JSON format */
        value = transform_schema2json(module, expr);
        free(expr);

        if (!value) {
            goto error;
        }
        rc = unres_schema_add_str(module, unres, type, UNRES_TYPE_IDENTREF, value);
        lydict_remove(module->ctx, value);

        if (rc == -1) {
            goto error;
        }
    }
    free(ref);

    return EXIT_SUCCESS;
error:
    for (i = i+1; i < size; ++i) {
        free(ref[i]);
    }
    free(ref);
    return EXIT_FAILURE;
}