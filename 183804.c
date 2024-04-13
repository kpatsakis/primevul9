yang_check_typedef(struct lys_module *module, struct lys_node *parent, struct unres_schema *unres)
{
    struct lys_tpdf *tpdf;
    uint8_t *ptr_tpdf_size = NULL;
    uint16_t j, i, tpdf_size, *ptr_tpdf_size16 = NULL;

    if (!parent) {
        tpdf = module->tpdf;
        //ptr_tpdf_size = &module->tpdf_size;
        ptr_tpdf_size16 = &module->tpdf_size;
    } else {
        switch (parent->nodetype) {
        case LYS_GROUPING:
            tpdf = ((struct lys_node_grp *)parent)->tpdf;
            ptr_tpdf_size16 = &((struct lys_node_grp *)parent)->tpdf_size;
            break;
        case LYS_CONTAINER:
            tpdf = ((struct lys_node_container *)parent)->tpdf;
            ptr_tpdf_size16 = &((struct lys_node_container *)parent)->tpdf_size;
            break;
        case LYS_LIST:
            tpdf = ((struct lys_node_list *)parent)->tpdf;
            ptr_tpdf_size = &((struct lys_node_list *)parent)->tpdf_size;
            break;
        case LYS_RPC:
        case LYS_ACTION:
            tpdf = ((struct lys_node_rpc_action *)parent)->tpdf;
            ptr_tpdf_size16 = &((struct lys_node_rpc_action *)parent)->tpdf_size;
            break;
        case LYS_INPUT:
        case LYS_OUTPUT:
            tpdf = ((struct lys_node_inout *)parent)->tpdf;
            ptr_tpdf_size16 = &((struct lys_node_inout *)parent)->tpdf_size;
            break;
        case LYS_NOTIF:
            tpdf = ((struct lys_node_notif *)parent)->tpdf;
            ptr_tpdf_size16 = &((struct lys_node_notif *)parent)->tpdf_size;
            break;
        default:
            LOGINT(module->ctx);
            return EXIT_FAILURE;
        }
    }

    if (ptr_tpdf_size16) {
        tpdf_size = *ptr_tpdf_size16;
        *ptr_tpdf_size16 = 0;
    } else {
        tpdf_size = *ptr_tpdf_size;
        *ptr_tpdf_size = 0;
    }

    for (i = 0; i < tpdf_size; ++i) {
        if (lyp_check_identifier(module->ctx, tpdf[i].name, LY_IDENT_TYPE, module, parent)) {
            goto error;
        }

        if (yang_fill_type(module, &tpdf[i].type, (struct yang_type *)tpdf[i].type.der, &tpdf[i], unres)) {
            goto error;
        }
        if (yang_check_ext_instance(module, &tpdf[i].ext, tpdf[i].ext_size, &tpdf[i], unres)) {
            goto error;
        }
        for (j = 0; j < tpdf[i].ext_size; ++j) {
            if (tpdf[i].ext[j]->flags & LYEXT_OPT_VALID) {
                tpdf[i].flags |= LYS_VALID_EXT;
                break;
            }
        }
        if (unres_schema_add_node(module, unres, &tpdf[i].type, UNRES_TYPE_DER_TPDF, parent) == -1) {
            goto error;
        }

        if (ptr_tpdf_size16) {
            (*ptr_tpdf_size16)++;
        } else {
            (*ptr_tpdf_size)++;
        }
        /* check default value*/
        if (!(module->ctx->models.flags & LY_CTX_TRUSTED)
                && unres_schema_add_node(module, unres, &tpdf[i].type, UNRES_TYPEDEF_DFLT, (struct lys_node *)(&tpdf[i].dflt)) == -1)  {
            ++i;
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    yang_tpdf_free(module->ctx, tpdf, i, tpdf_size);
    return EXIT_FAILURE;
}