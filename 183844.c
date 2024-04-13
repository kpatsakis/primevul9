yang_read_common(struct lys_module *module, char *value, enum yytokentype type)
{
    int ret = 0;

    switch (type) {
    case MODULE_KEYWORD:
        module->name = lydict_insert_zc(module->ctx, value);
        break;
    case NAMESPACE_KEYWORD:
        ret = yang_check_string(module, &module->ns, "namespace", "module", value, NULL);
        break;
    case ORGANIZATION_KEYWORD:
        ret = yang_check_string(module, &module->org, "organization", "module", value, NULL);
        break;
    case CONTACT_KEYWORD:
        ret = yang_check_string(module, &module->contact, "contact", "module", value, NULL);
        break;
    default:
        free(value);
        LOGINT(module->ctx);
        ret = EXIT_FAILURE;
        break;
    }

    return ret;
}