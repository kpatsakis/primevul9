lyp_check_status(uint16_t flags1, struct lys_module *mod1, const char *name1,
                 uint16_t flags2, struct lys_module *mod2, const char *name2,
                 const struct lys_node *node)
{
    uint16_t flg1, flg2;

    flg1 = (flags1 & LYS_STATUS_MASK) ? (flags1 & LYS_STATUS_MASK) : LYS_STATUS_CURR;
    flg2 = (flags2 & LYS_STATUS_MASK) ? (flags2 & LYS_STATUS_MASK) : LYS_STATUS_CURR;

    if ((flg1 < flg2) && (lys_main_module(mod1) == lys_main_module(mod2))) {
        LOGVAL(mod1->ctx, LYE_INSTATUS, node ? LY_VLOG_LYS : LY_VLOG_NONE, node,
               flg1 == LYS_STATUS_CURR ? "current" : "deprecated", name1, "references",
               flg2 == LYS_STATUS_OBSLT ? "obsolete" : "deprecated", name2);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}