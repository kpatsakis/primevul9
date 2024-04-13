yang_read_presence(struct lys_module *module, struct lys_node_container *cont, char *value)
{
    if (cont->presence) {
        LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, cont, "presence", "container");
        free(value);
        return EXIT_FAILURE;
    } else {
        cont->presence = lydict_insert_zc(module->ctx, value);
        return EXIT_SUCCESS;
    }
}