yang_extcomplex_node(struct lys_ext_instance_complex *ext, char *parent_name, char *node_name,
                     struct lys_node *node, LY_STMT stmt)
{
    struct lyext_substmt *info;
    struct lys_node **snode, *siter;

    snode = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!snode) {
        LOGVAL(ext->module->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node_name, parent_name);
        return EXIT_FAILURE;
    }
    if (info->cardinality < LY_STMT_CARD_SOME) {
        LY_TREE_FOR(node, siter) {
            if (stmt == lys_snode2stmt(siter->nodetype)) {
                LOGVAL(ext->module->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node_name, parent_name);
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}