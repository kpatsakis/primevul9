yang_fill_import(struct lys_module *module, struct lys_import *imp_old, struct lys_import *imp_new,
                 char *value, struct unres_schema *unres)
{
    const char *exp;
    int rc;

    if (!imp_old->prefix) {
        LOGVAL(module->ctx, LYE_MISSCHILDSTMT, LY_VLOG_NONE, NULL, "prefix", "import");
        goto error;
    } else {
        if (lyp_check_identifier(module->ctx, imp_old->prefix, LY_IDENT_PREFIX, module, NULL)) {
            goto error;
        }
    }
    memcpy(imp_new, imp_old, sizeof *imp_old);
    exp = lydict_insert_zc(module->ctx, value);
    rc = lyp_check_import(module, exp, imp_new);
    lydict_remove(module->ctx, exp);
    module->imp_size++;
    if (rc || yang_check_ext_instance(module, &imp_new->ext, imp_new->ext_size, imp_new, unres)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

error:
    free(value);
    lydict_remove(module->ctx, imp_old->dsc);
    lydict_remove(module->ctx, imp_old->ref);
    lydict_remove(module->ctx, imp_old->prefix);
    lys_extension_instances_free(module->ctx, imp_old->ext, imp_old->ext_size, NULL);
    return EXIT_FAILURE;
}