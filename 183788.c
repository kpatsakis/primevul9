yang_check_nodes(struct lys_module *module, struct lys_node *parent, struct lys_node *nodes,
                 int options, struct unres_schema *unres)
{
    struct lys_node *node = nodes, *sibling, *child;
    int i;

    while (node) {
        sibling = node->next;
        child = node->child;
        node->next = NULL;
        node->child = NULL;
        node->parent = NULL;
        node->prev = node;

        if (lys_node_addchild(parent, module->type ? ((struct lys_submodule *)module)->belongsto: module, node, 0) ||
            check_status_flag(node, parent)) {
            lys_node_unlink(node);
            yang_free_nodes(module->ctx, node);
            goto error;
        }
        if (node->parent != parent) {
            assert(node->parent->parent == parent);
            assert((node->parent->nodetype == LYS_CASE) && (node->parent->flags & LYS_IMPLICIT));
            store_config_flag(node->parent, options);
        }
        store_config_flag(node, options);
        if (yang_check_ext_instance(module, &node->ext, node->ext_size, node, unres)) {
            goto error;
        }
        for (i = 0; i < node->ext_size; ++i) {
            if (node->ext[i]->flags & LYEXT_OPT_VALID) {
                node->flags |= LYS_VALID_EXT;
                break;
            }
        }

        switch (node->nodetype) {
        case LYS_GROUPING:
            if (yang_check_typedef(module, node, unres)) {
                goto error;
            }
            if (yang_check_iffeatures(module, NULL, node, GROUPING_KEYWORD, unres)) {
                goto error;
            }
            if (yang_check_nodes(module, node, child, options | LYS_PARSE_OPT_INGRP, unres)) {
                child = NULL;
                goto error;
            }
            break;
        case LYS_CONTAINER:
            if (yang_check_container(module, (struct lys_node_container *)node, &child, options, unres)) {
                goto error;
            }
            break;
        case LYS_LEAF:
            if (yang_check_leaf(module, (struct lys_node_leaf *)node, options, unres)) {
                child = NULL;
                goto error;
            }
            break;
        case LYS_LEAFLIST:
            if (yang_check_leaflist(module, (struct lys_node_leaflist *)node, options, unres)) {
                child = NULL;
                goto error;
            }
            break;
        case LYS_LIST:
            if (yang_check_list(module, (struct lys_node_list *)node, &child, options, unres)) {
                goto error;
            }
            break;
        case LYS_CHOICE:
            if (yang_check_choice(module, (struct lys_node_choice *)node, &child, options, unres)) {
                goto error;
            }
            break;
        case LYS_CASE:
            if (yang_check_iffeatures(module, NULL, node, CASE_KEYWORD, unres)) {
                goto error;
            }
            if (yang_check_nodes(module, node, child, options, unres)) {
                child = NULL;
                goto error;
            }
            if (((struct lys_node_case *)node)->when) {
                if (yang_check_ext_instance(module, &((struct lys_node_case *)node)->when->ext,
                        ((struct lys_node_case *)node)->when->ext_size, ((struct lys_node_case *)node)->when, unres)) {
                    goto error;
                }
                /* check XPath dependencies */
                if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && (options & LYS_PARSE_OPT_INGRP)) {
                    if (lyxp_node_check_syntax(node)) {
                        goto error;
                    }
                } else {
                    if (unres_schema_add_node(module, unres, node, UNRES_XPATH, NULL) == -1) {
                        goto error;
                    }
                }
            }
            break;
        case LYS_ANYDATA:
        case LYS_ANYXML:
            if (yang_check_anydata(module, (struct lys_node_anydata *)node, &child, options, unres)) {
                goto error;
            }
            break;
        case LYS_RPC:
        case LYS_ACTION:
            if (yang_check_rpc_action(module, (struct lys_node_rpc_action *)node, &child, options, unres)){
                goto error;
            }
            break;
        case LYS_INPUT:
        case LYS_OUTPUT:
            if (yang_check_typedef(module, node, unres)) {
                goto error;
            }
            if (yang_check_nodes(module, node, child, options, unres)) {
                child = NULL;
                goto error;
            }
            if (((struct lys_node_inout *)node)->must_size) {
                if (yang_check_must(module, ((struct lys_node_inout *)node)->must, ((struct lys_node_inout *)node)->must_size, unres)) {
                    goto error;
                }
                /* check XPath dependencies */
                if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && (options & LYS_PARSE_OPT_INGRP)) {
                    if (lyxp_node_check_syntax(node)) {
                        goto error;
                    }
                } else {
                    if (unres_schema_add_node(module, unres, node, UNRES_XPATH, NULL) == -1) {
                        goto error;
                    }
                }
            }
            break;
        case LYS_NOTIF:
            if (yang_check_notif(module, (struct lys_node_notif *)node, &child, options, unres)) {
                goto error;
            }
            break;
        case LYS_USES:
            if (yang_check_uses(module, (struct lys_node_uses *)node, options, unres)) {
                child = NULL;
                goto error;
            }
            break;
        default:
            LOGINT(module->ctx);
            goto error;
        }
        node = sibling;
    }

    return EXIT_SUCCESS;

error:
    yang_free_nodes(module->ctx, sibling);
    yang_free_nodes(module->ctx, child);
    return EXIT_FAILURE;
}