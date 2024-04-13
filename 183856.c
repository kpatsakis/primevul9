yang_check_deviate_must(struct lys_module *module, struct unres_schema *unres,
                        struct lys_deviate *deviate, struct lys_node *dev_target)
{
    struct ly_ctx *ctx = module->ctx;
    int i, j, erase_must = 1;
    struct lys_restr **trg_must, *must;
    uint8_t *trg_must_size;

    /* check target node type */
    switch (dev_target->nodetype) {
        case LYS_LEAF:
            trg_must = &((struct lys_node_leaf *)dev_target)->must;
            trg_must_size = &((struct lys_node_leaf *)dev_target)->must_size;
            break;
        case LYS_CONTAINER:
            trg_must = &((struct lys_node_container *)dev_target)->must;
            trg_must_size = &((struct lys_node_container *)dev_target)->must_size;
            break;
        case LYS_LEAFLIST:
            trg_must = &((struct lys_node_leaflist *)dev_target)->must;
            trg_must_size = &((struct lys_node_leaflist *)dev_target)->must_size;
            break;
        case LYS_LIST:
            trg_must = &((struct lys_node_list *)dev_target)->must;
            trg_must_size = &((struct lys_node_list *)dev_target)->must_size;
            break;
        case LYS_ANYXML:
        case LYS_ANYDATA:
            trg_must = &((struct lys_node_anydata *)dev_target)->must;
            trg_must_size = &((struct lys_node_anydata *)dev_target)->must_size;
            break;
        default:
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "must");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"must\" property.");
            goto error;
    }

    /* flag will be checked again, clear it for now */
    dev_target->flags &= ~(LYS_XPCONF_DEP | LYS_XPSTATE_DEP);

    if (deviate->mod == LY_DEVIATE_ADD) {
        /* reallocate the must array of the target */
        must = ly_realloc(*trg_must, (deviate->must_size + *trg_must_size) * sizeof *must);
        LY_CHECK_ERR_GOTO(!must, LOGMEM(ctx), error);
        *trg_must = must;
        memcpy(&(*trg_must)[*trg_must_size], deviate->must, deviate->must_size * sizeof *must);
        *trg_must_size = *trg_must_size + deviate->must_size;
        erase_must = 0;
    } else if (deviate->mod == LY_DEVIATE_DEL) {
        /* find must to delete, we are ok with just matching conditions */
        for (j = 0; j < deviate->must_size; ++j) {
            for (i = 0; i < *trg_must_size; i++) {
                if (ly_strequal(deviate->must[j].expr, (*trg_must)[i].expr, 1)) {
                    /* we have a match, free the must structure ... */
                    lys_restr_free(module->ctx, &((*trg_must)[i]), NULL);
                    /* ... and maintain the array */
                    (*trg_must_size)--;
                    if (i != *trg_must_size) {
                        memcpy(&(*trg_must)[i], &(*trg_must)[*trg_must_size], sizeof *must);
                    }
                    if (!(*trg_must_size)) {
                        free(*trg_must);
                        *trg_must = NULL;
                    } else {
                        memset(&(*trg_must)[*trg_must_size], 0, sizeof *must);
                    }

                    i = -1; /* set match flag */
                    break;
                }
            }
            if (i != -1) {
                /* no match found */
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, deviate->must[j].expr, "must");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value does not match any must from the target.");
                goto error;
            }
        }
    }

    if (yang_check_must(module, deviate->must, deviate->must_size, unres)) {
        goto error;
    }
    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && *trg_must_size
            && (unres_schema_add_node(module, unres, dev_target, UNRES_XPATH, NULL) == -1)) {
        goto error;
    }

    return EXIT_SUCCESS;

error:
    if (deviate->mod == LY_DEVIATE_ADD && erase_must) {
        for (i = 0; i < deviate->must_size; ++i) {
            lys_restr_free(module->ctx, &deviate->must[i], NULL);
        }
        free(deviate->must);
    }
    return EXIT_FAILURE;
}