resolve_iffeature_compile(struct lys_iffeature *iffeat_expr, const char *value, struct lys_node *node,
                          int infeature, struct unres_schema *unres)
{
    const char *c = value;
    int r, rc = EXIT_FAILURE;
    int i, j, last_not, checkversion = 0;
    unsigned int f_size = 0, expr_size = 0, f_exp = 1;
    uint8_t op;
    struct iff_stack stack = {0, 0, NULL};
    struct unres_iffeat_data *iff_data;
    struct ly_ctx *ctx = node->module->ctx;

    assert(c);

    if (isspace(c[0])) {
        LOGVAL(ctx, LYE_INCHAR, LY_VLOG_NONE, NULL, c[0], c);
        return EXIT_FAILURE;
    }

    /* pre-parse the expression to get sizes for arrays, also do some syntax checks of the expression */
    for (i = j = last_not = 0; c[i]; i++) {
        if (c[i] == '(') {
            checkversion = 1;
            j++;
            continue;
        } else if (c[i] == ')') {
            j--;
            continue;
        } else if (isspace(c[i])) {
            continue;
        }

        if (!strncmp(&c[i], "not", r = 3) || !strncmp(&c[i], "and", r = 3) || !strncmp(&c[i], "or", r = 2)) {
            if (c[i + r] == '\0') {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "if-feature");
                return EXIT_FAILURE;
            } else if (!isspace(c[i + r])) {
                /* feature name starting with the not/and/or */
                last_not = 0;
                f_size++;
            } else if (c[i] == 'n') { /* not operation */
                if (last_not) {
                    /* double not */
                    expr_size = expr_size - 2;
                    last_not = 0;
                } else {
                    last_not = 1;
                }
            } else { /* and, or */
                f_exp++;
                /* not a not operation */
                last_not = 0;
            }
            i += r;
        } else {
            f_size++;
            last_not = 0;
        }
        expr_size++;

        while (!isspace(c[i])) {
            if (c[i] == '(') {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "if-feature");
                return EXIT_FAILURE;
            } else if (!c[i] || c[i] == ')') {
                i--;
                break;
            }
            i++;
        }
    }
    if (j || f_exp != f_size) {
        /* not matching count of ( and ) */
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "if-feature");
        return EXIT_FAILURE;
    }

    if (checkversion || expr_size > 1) {
        /* check that we have 1.1 module */
        if (node->module->version != LYS_VERSION_1_1) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "if-feature");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "YANG 1.1 if-feature expression found in 1.0 module.");
            return EXIT_FAILURE;
        }
    }

    /* allocate the memory */
    iffeat_expr->expr = calloc((j = (expr_size / 4) + ((expr_size % 4) ? 1 : 0)), sizeof *iffeat_expr->expr);
    iffeat_expr->features = calloc(f_size, sizeof *iffeat_expr->features);
    stack.stack = malloc(expr_size * sizeof *stack.stack);
    LY_CHECK_ERR_GOTO(!stack.stack || !iffeat_expr->expr || !iffeat_expr->features, LOGMEM(ctx), error);
    stack.size = expr_size;
    f_size--; expr_size--; /* used as indexes from now */

    for (i--; i >= 0; i--) {
        if (c[i] == ')') {
            /* push it on stack */
            iff_stack_push(&stack, LYS_IFF_RP);
            continue;
        } else if (c[i] == '(') {
            /* pop from the stack into result all operators until ) */
            while((op = iff_stack_pop(&stack)) != LYS_IFF_RP) {
                iff_setop(iffeat_expr->expr, op, expr_size--);
            }
            continue;
        } else if (isspace(c[i])) {
            continue;
        }

        /* end operator or operand -> find beginning and get what is it */
        j = i + 1;
        while (i >= 0 && !isspace(c[i]) && c[i] != '(') {
            i--;
        }
        i++; /* get back by one step */

        if (!strncmp(&c[i], "not", 3) && isspace(c[i + 3])) {
            if (stack.index && stack.stack[stack.index - 1] == LYS_IFF_NOT) {
                /* double not */
                iff_stack_pop(&stack);
            } else {
                /* not has the highest priority, so do not pop from the stack
                 * as in case of AND and OR */
                iff_stack_push(&stack, LYS_IFF_NOT);
            }
        } else if (!strncmp(&c[i], "and", 3) && isspace(c[i + 3])) {
            /* as for OR - pop from the stack all operators with the same or higher
             * priority and store them to the result, then push the AND to the stack */
            while (stack.index && stack.stack[stack.index - 1] <= LYS_IFF_AND) {
                op = iff_stack_pop(&stack);
                iff_setop(iffeat_expr->expr, op, expr_size--);
            }
            iff_stack_push(&stack, LYS_IFF_AND);
        } else if (!strncmp(&c[i], "or", 2) && isspace(c[i + 2])) {
            while (stack.index && stack.stack[stack.index - 1] <= LYS_IFF_OR) {
                op = iff_stack_pop(&stack);
                iff_setop(iffeat_expr->expr, op, expr_size--);
            }
            iff_stack_push(&stack, LYS_IFF_OR);
        } else {
            /* feature name, length is j - i */

            /* add it to the result */
            iff_setop(iffeat_expr->expr, LYS_IFF_F, expr_size--);

            /* now get the link to the feature definition. Since it can be
             * forward referenced, we have to keep the feature name in auxiliary
             * structure passed into unres */
            iff_data = malloc(sizeof *iff_data);
            LY_CHECK_ERR_GOTO(!iff_data, LOGMEM(ctx), error);
            iff_data->node = node;
            iff_data->fname = lydict_insert(node->module->ctx, &c[i], j - i);
            iff_data->infeature = infeature;
            r = unres_schema_add_node(node->module, unres, &iffeat_expr->features[f_size], UNRES_IFFEAT,
                                      (struct lys_node *)iff_data);
            f_size--;

            if (r == -1) {
                lydict_remove(node->module->ctx, iff_data->fname);
                free(iff_data);
                goto error;
            }
        }
    }
    while (stack.index) {
        op = iff_stack_pop(&stack);
        iff_setop(iffeat_expr->expr, op, expr_size--);
    }

    if (++expr_size || ++f_size) {
        /* not all expected operators and operands found */
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "if-feature");
        rc = EXIT_FAILURE;
    } else {
        rc = EXIT_SUCCESS;
    }

error:
    /* cleanup */
    iff_stack_clean(&stack);

    return rc;
}