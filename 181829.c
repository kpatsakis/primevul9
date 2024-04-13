lyp_check_mandatory_(const struct lys_node *root)
{
    int mand_flag = 0;
    const struct lys_node *iter = NULL;

    while ((iter = lys_getnext(iter, root, NULL, LYS_GETNEXT_WITHCHOICE | LYS_GETNEXT_WITHUSES | LYS_GETNEXT_INTOUSES
            | LYS_GETNEXT_INTONPCONT | LYS_GETNEXT_NOSTATECHECK))) {
        if (iter->nodetype == LYS_USES) {
            if (!((struct lys_node_uses *)iter)->grp) {
                /* not yet resolved uses */
                return root;
            } else {
                /* go into uses */
                continue;
            }
        }
        if (iter->nodetype == LYS_CHOICE) {
            /* skip it, it was already checked for direct mandatory node in default */
            continue;
        }
        if (iter->nodetype == LYS_LIST) {
            if (((struct lys_node_list *)iter)->min) {
                mand_flag = 1;
            }
        } else if (iter->nodetype == LYS_LEAFLIST) {
            if (((struct lys_node_leaflist *)iter)->min) {
                mand_flag = 1;
            }
        } else if (iter->flags & LYS_MAND_TRUE) {
            mand_flag = 1;
        }

        if (mand_flag) {
            return iter;
        }
    }

    return NULL;
}