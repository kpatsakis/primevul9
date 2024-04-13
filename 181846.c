lyp_check_length_range(struct ly_ctx *ctx, const char *expr, struct lys_type *type)
{
    struct len_ran_intv *intv = NULL, *tmp_intv;
    const char *c = expr, *tail;
    int ret = EXIT_FAILURE, flg = 1; /* first run flag */

    assert(expr);

lengthpart:

    while (isspace(*c)) {
        c++;
    }

    /* lower boundary or explicit number */
    if (!strncmp(c, "max", 3)) {
max:
        c += 3;
        while (isspace(*c)) {
            c++;
        }
        if (*c != '\0') {
            goto error;
        }

        goto syntax_ok;

    } else if (!strncmp(c, "min", 3)) {
        if (!flg) {
            /* min cannot be used elsewhere than in the first length-part */
            goto error;
        } else {
            flg = 0;
        }
        c += 3;
        while (isspace(*c)) {
            c++;
        }

        if (*c == '|') {
            c++;
            /* process next length-part */
            goto lengthpart;
        } else if (*c == '\0') {
            goto syntax_ok;
        } else if (!strncmp(c, "..", 2)) {
upper:
            c += 2;
            while (isspace(*c)) {
                c++;
            }
            if (*c == '\0') {
                goto error;
            }

            /* upper boundary */
            if (!strncmp(c, "max", 3)) {
                goto max;
            }

            check_number(c, &tail, type->base);
            if (c == tail) {
                goto error;
            }
            c = tail;
            while (isspace(*c)) {
                c++;
            }
            if (*c == '\0') {
                goto syntax_ok;
            } else if (*c == '|') {
                c++;
                /* process next length-part */
                goto lengthpart;
            } else {
                goto error;
            }
        } else {
            goto error;
        }

    } else if (isdigit(*c) || (*c == '-') || (*c == '+')) {
        /* number */
        check_number(c, &tail, type->base);
        if (c == tail) {
            goto error;
        }
        c = tail;

        while (isspace(*c)) {
            c++;
        }

        if (*c == '|') {
            c++;
            /* process next length-part */
            goto lengthpart;
        } else if (*c == '\0') {
            goto syntax_ok;
        } else if (!strncmp(c, "..", 2)) {
            goto upper;
        }

    } else {
        goto error;
    }

syntax_ok:
    if (resolve_len_ran_interval(ctx, expr, type, &intv)) {
        goto error;
    }

    ret = EXIT_SUCCESS;

error:
    while (intv) {
        tmp_intv = intv->next;
        free(intv);
        intv = tmp_intv;
    }

    return ret;
}