yang_read_type(struct ly_ctx *ctx, void *parent, char *value, enum yytokentype type)
{
    struct yang_type *typ;
    struct lys_deviate *dev;

    typ = calloc(1, sizeof *typ);
    LY_CHECK_ERR_RETURN(!typ, LOGMEM(ctx), NULL);

    typ->flags = LY_YANG_STRUCTURE_FLAG;
    switch (type) {
    case LEAF_KEYWORD:
        if (((struct lys_node_leaf *)parent)->type.der) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYS, parent, "type", "leaf");
            goto error;
        }
        ((struct lys_node_leaf *)parent)->type.der = (struct lys_tpdf *)typ;
        ((struct lys_node_leaf *)parent)->type.parent = (struct lys_tpdf *)parent;
        typ->type = &((struct lys_node_leaf *)parent)->type;
        break;
    case LEAF_LIST_KEYWORD:
        if (((struct lys_node_leaflist *)parent)->type.der) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_LYS, parent, "type", "leaf-list");
            goto error;
        }
        ((struct lys_node_leaflist *)parent)->type.der = (struct lys_tpdf *)typ;
        ((struct lys_node_leaflist *)parent)->type.parent = (struct lys_tpdf *)parent;
        typ->type = &((struct lys_node_leaflist *)parent)->type;
        break;
    case UNION_KEYWORD:
        ((struct lys_type *)parent)->der = (struct lys_tpdf *)typ;
        typ->type = (struct lys_type *)parent;
        break;
    case TYPEDEF_KEYWORD:
        if (((struct lys_tpdf *)parent)->type.der) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "type", "typedef");
            goto error;
        }
        ((struct lys_tpdf *)parent)->type.der = (struct lys_tpdf *)typ;
        typ->type = &((struct lys_tpdf *)parent)->type;
        break;
    case REPLACE_KEYWORD:
        /* deviation replace type*/
        dev = (struct lys_deviate *)parent;
        if (dev->type) {
            LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "type", "deviation");
            goto error;
        }
        dev->type = calloc(1, sizeof *dev->type);
        LY_CHECK_ERR_GOTO(!dev->type, LOGMEM(ctx), error);
        dev->type->der = (struct lys_tpdf *)typ;
        typ->type = dev->type;
        break;
    case EXTENSION_INSTANCE:
        ((struct lys_type *)parent)->der = (struct lys_tpdf *)typ;
        typ->type = parent;
        break;
    default:
        goto error;
        break;
    }
    typ->name = lydict_insert_zc(ctx, value);
    return typ;

error:
    free(value);
    free(typ);
    return NULL;
}