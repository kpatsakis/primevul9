parse_uint(const char *val_str, uint64_t max, int base, uint64_t *ret, struct lyd_node *node)
{
    char *strptr;
    uint64_t u;

    assert(node);

    if (!val_str || !val_str[0]) {
        goto error;
    }

    errno = 0;
    strptr = NULL;
    u = strtoull(val_str, &strptr, base);
    if (errno || (u > max)) {
        goto error;
    } else if (strptr && *strptr) {
        while (isspace(*strptr)) {
            ++strptr;
        }
        if (*strptr) {
            goto error;
        }
    } else if (u != 0 && val_str[0] == '-') {
        goto error;
    }

    *ret = u;
    return EXIT_SUCCESS;

error:
    LOGVAL(node->schema->module->ctx, LYE_INVAL, LY_VLOG_LYD, node, val_str ? val_str : "", node->schema->name);
    return EXIT_FAILURE;
}