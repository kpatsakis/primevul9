yang_read_prefix(struct lys_module *module, struct lys_import *imp, char *value)
{
    int ret = 0;

    if (!imp && lyp_check_identifier(module->ctx, value, LY_IDENT_PREFIX, module, NULL)) {
        free(value);
        return EXIT_FAILURE;
    }

    if (imp) {
        ret = yang_check_string(module, &imp->prefix, "prefix", "import", value, NULL);
    } else {
        ret = yang_check_string(module, &module->prefix, "prefix", "module", value, NULL);
    }

    return ret;
}