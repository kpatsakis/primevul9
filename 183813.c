yang_fill_extcomplex_flags(struct lys_ext_instance_complex *ext, char *parent_name, char *node_name,
                           LY_STMT stmt, uint16_t value, uint16_t mask)
{
    uint16_t *data;
    struct lyext_substmt *info;

    data = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!data) {
        LOGVAL(ext->module->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node_name, parent_name);
        return EXIT_FAILURE;
    }
    if (info->cardinality < LY_STMT_CARD_SOME && (*data & mask)) {
        LOGVAL(ext->module->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node_name, parent_name);
        return EXIT_FAILURE;
    }

    *data |= value;
    return EXIT_SUCCESS;
}