lyp_check_edit_attr(struct ly_ctx *ctx, struct lyd_attr *attr, struct lyd_node *parent, int *editbits)
{
    struct lyd_attr *last = NULL;
    int bits = 0;

    /* 0x01 - insert attribute present
     * 0x02 - insert is relative (before or after)
     * 0x04 - value attribute present
     * 0x08 - key attribute present
     * 0x10 - operation attribute present
     * 0x20 - operation not allowing insert attribute (delete or remove)
     */
    LY_TREE_FOR(attr, attr) {
        last = NULL;
        if (!strcmp(attr->annotation->arg_value, "operation") &&
                !strcmp(attr->annotation->module->name, "ietf-netconf")) {
            if (bits & 0x10) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYD, parent, "operation attributes", parent->schema->name);
                return -1;
            }

            bits |= 0x10;
            if (attr->value.enm->value >= 3) {
                /* delete or remove */
                bits |= 0x20;
            }
        } else if (attr->annotation->module == ctx->models.list[1] && /* internal YANG schema */
                !strcmp(attr->annotation->arg_value, "insert")) {
            /* 'insert' attribute present */
            if (!(parent->schema->flags & LYS_USERORDERED)) {
                /* ... but it is not expected */
                LOGVAL(ctx, LYE_INATTR, LY_VLOG_LYD, parent, "insert");
                return -1;
            }
            if (bits & 0x01) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYD, parent, "insert attributes", parent->schema->name);
                return -1;
            }

            bits |= 0x01;
            if (attr->value.enm->value >= 2) {
                /* before or after */
                bits |= 0x02;
            }
            last = attr;
        } else if (attr->annotation->module == ctx->models.list[1] && /* internal YANG schema */
                !strcmp(attr->annotation->arg_value, "value")) {
            if (bits & 0x04) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYD, parent, "value attributes", parent->schema->name);
                return -1;
            } else if (parent->schema->nodetype & LYS_LIST) {
                LOGVAL(ctx, LYE_INATTR, LY_VLOG_LYD, parent, attr->name);
                return -1;
            }
            bits |= 0x04;
            last = attr;
        } else if (attr->annotation->module == ctx->models.list[1] && /* internal YANG schema */
                !strcmp(attr->annotation->arg_value, "key")) {
            if (bits & 0x08) {
                LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYD, parent, "key attributes", parent->schema->name);
                return -1;
            } else if (parent->schema->nodetype & LYS_LEAFLIST) {
                LOGVAL(ctx, LYE_INATTR, LY_VLOG_LYD, parent, attr->name);
                return -1;
            }
            bits |= 0x08;
            last = attr;
        }
    }

    /* report errors */
    if (last && (!(parent->schema->nodetype & (LYS_LEAFLIST | LYS_LIST)) || !(parent->schema->flags & LYS_USERORDERED))) {
        /* moving attributes in wrong elements (not an user ordered list or not a list at all) */
        LOGVAL(ctx, LYE_INATTR, LY_VLOG_LYD, parent, last->name);
        return -1;
    } else if (bits == 3) {
        /* 0x01 | 0x02 - relative position, but value/key is missing */
        if (parent->schema->nodetype & LYS_LIST) {
            LOGVAL(ctx, LYE_MISSATTR, LY_VLOG_LYD, parent, "key", parent->schema->name);
        } else { /* LYS_LEAFLIST */
            LOGVAL(ctx, LYE_MISSATTR, LY_VLOG_LYD, parent, "value", parent->schema->name);
        }
        return -1;
    } else if ((bits & (0x04 | 0x08)) && !(bits & 0x02)) {
        /* key/value without relative position */
        LOGVAL(ctx, LYE_INATTR, LY_VLOG_LYD, parent, (bits & 0x04) ? "value" : "key");
        return -1;
    } else if ((bits & 0x21) == 0x21) {
        /* insert in delete/remove */
        LOGVAL(ctx, LYE_INATTR, LY_VLOG_LYD, parent, "insert");
        return -1;
    }

    if (editbits) {
        *editbits = bits;
    }
    return 0;
}