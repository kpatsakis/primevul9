yang_check_string(struct lys_module *module, const char **target, char *what,
                  char *where, char *value, struct lys_node *node)
{
    if (*target) {
        LOGVAL(module->ctx, LYE_TOOMANY, (node) ? LY_VLOG_LYS : LY_VLOG_NONE, node, what, where);
        free(value);
        return 1;
    } else {
        *target = lydict_insert_zc(module->ctx, value);
        return 0;
    }
}