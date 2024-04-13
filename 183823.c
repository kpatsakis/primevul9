yang_read_when(struct lys_module *module, struct lys_node *node, enum yytokentype type, char *value)
{
    struct lys_when *retval;

    retval = calloc(1, sizeof *retval);
    LY_CHECK_ERR_RETURN(!retval, LOGMEM(module->ctx); free(value), NULL);
    retval->cond = transform_schema2json(module, value);
    if (!retval->cond) {
        goto error;
    }
    switch (type) {
    case CONTAINER_KEYWORD:
        if (((struct lys_node_container *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "container");
            goto error;
        }
        ((struct lys_node_container *)node)->when = retval;
        break;
    case ANYDATA_KEYWORD:
    case ANYXML_KEYWORD:
        if (((struct lys_node_anydata *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", (type == ANYXML_KEYWORD) ? "anyxml" : "anydata");
            goto error;
        }
        ((struct lys_node_anydata *)node)->when = retval;
        break;
    case CHOICE_KEYWORD:
        if (((struct lys_node_choice *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "choice");
            goto error;
        }
        ((struct lys_node_choice *)node)->when = retval;
        break;
    case CASE_KEYWORD:
        if (((struct lys_node_case *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "case");
            goto error;
        }
        ((struct lys_node_case *)node)->when = retval;
        break;
    case LEAF_KEYWORD:
        if (((struct lys_node_leaf *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "leaf");
            goto error;
        }
        ((struct lys_node_leaf *)node)->when = retval;
        break;
    case LEAF_LIST_KEYWORD:
        if (((struct lys_node_leaflist *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "leaflist");
            goto error;
        }
        ((struct lys_node_leaflist *)node)->when = retval;
        break;
    case LIST_KEYWORD:
        if (((struct lys_node_list *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "list");
            goto error;
        }
        ((struct lys_node_list *)node)->when = retval;
        break;
    case USES_KEYWORD:
        if (((struct lys_node_uses *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "uses");
            goto error;
        }
        ((struct lys_node_uses *)node)->when = retval;
        break;
    case AUGMENT_KEYWORD:
        if (((struct lys_node_augment *)node)->when) {
            LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_LYS, node, "when", "augment");
            goto error;
        }
        ((struct lys_node_augment *)node)->when = retval;
        break;
    case EXTENSION_INSTANCE:
        *(struct lys_when **)node = retval;
        break;
    default:
        goto error;
        break;
    }
    free(value);
    return retval;

error:
    free(value);
    lydict_remove(module->ctx, retval->cond);
    free(retval);
    return NULL;
}