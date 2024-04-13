yang_check_deviate_mandatory(struct lys_deviate *deviate, struct lys_node *dev_target)
{
    struct ly_ctx *ctx = dev_target->module->ctx;
    struct lys_node *parent;

    /* check target node type */
    if (!(dev_target->nodetype & (LYS_LEAF | LYS_CHOICE | LYS_ANYDATA))) {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "mandatory");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"mandatory\" property.");
        goto error;
    }

    if (deviate->mod == LY_DEVIATE_ADD) {
        /* check that there is no current value */
        if (dev_target->flags & LYS_MAND_MASK) {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "mandatory");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
            goto error;
        } else {
            if (dev_target->nodetype == LYS_LEAF && ((struct lys_node_leaf *)dev_target)->dflt) {
                /* RFC 6020, 7.6.4 - default statement must not with mandatory true */
                LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "mandatory", "leaf");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "The \"mandatory\" statement is forbidden on leaf with \"default\".");
                goto error;
            } else if (dev_target->nodetype == LYS_CHOICE && ((struct lys_node_choice *)dev_target)->dflt) {
                LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "mandatory", "choice");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "The \"mandatory\" statement is forbidden on choices with \"default\".");
                goto error;
            }
        }
    } else { /* replace */
        if (!(dev_target->flags & LYS_MAND_MASK)) {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "mandatory");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Replacing a property that does not exist.");
            goto error;
        }
    }

    /* remove current mandatory value of the target ... */
    dev_target->flags &= ~LYS_MAND_MASK;

    /* ... and replace it with the value specified in deviation */
    dev_target->flags |= deviate->flags & LYS_MAND_MASK;

    /* check for mandatory node in default case, first find the closest parent choice to the changed node */
    for (parent = dev_target->parent;
         parent && !(parent->nodetype & (LYS_CHOICE | LYS_GROUPING | LYS_ACTION));
         parent = parent->parent) {
        if (parent->nodetype == LYS_CONTAINER && ((struct lys_node_container *)parent)->presence) {
            /* stop also on presence containers */
            break;
        }
    }
    /* and if it is a choice with the default case, check it for presence of a mandatory node in it */
    if (parent && parent->nodetype == LYS_CHOICE && ((struct lys_node_choice *)parent)->dflt) {
        if (lyp_check_mandatory_choice(parent)) {
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}