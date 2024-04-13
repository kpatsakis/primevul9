resolve_len_ran_interval(struct ly_ctx *ctx, const char *str_restr, struct lys_type *type, struct len_ran_intv **ret)
{
    /* 0 - unsigned, 1 - signed, 2 - floating point */
    int kind;
    int64_t local_smin = 0, local_smax = 0, local_fmin, local_fmax;
    uint64_t local_umin, local_umax = 0;
    uint8_t local_fdig = 0;
    const char *seg_ptr, *ptr;
    struct len_ran_intv *local_intv = NULL, *tmp_local_intv = NULL, *tmp_intv, *intv = NULL;

    switch (type->base) {
    case LY_TYPE_BINARY:
        kind = 0;
        local_umin = 0;
        local_umax = 18446744073709551615UL;

        if (!str_restr && type->info.binary.length) {
            str_restr = type->info.binary.length->expr;
        }
        break;
    case LY_TYPE_DEC64:
        kind = 2;
        local_fmin = __INT64_C(-9223372036854775807) - __INT64_C(1);
        local_fmax = __INT64_C(9223372036854775807);
        local_fdig = type->info.dec64.dig;

        if (!str_restr && type->info.dec64.range) {
            str_restr = type->info.dec64.range->expr;
        }
        break;
    case LY_TYPE_INT8:
        kind = 1;
        local_smin = __INT64_C(-128);
        local_smax = __INT64_C(127);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_INT16:
        kind = 1;
        local_smin = __INT64_C(-32768);
        local_smax = __INT64_C(32767);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_INT32:
        kind = 1;
        local_smin = __INT64_C(-2147483648);
        local_smax = __INT64_C(2147483647);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_INT64:
        kind = 1;
        local_smin = __INT64_C(-9223372036854775807) - __INT64_C(1);
        local_smax = __INT64_C(9223372036854775807);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_UINT8:
        kind = 0;
        local_umin = __UINT64_C(0);
        local_umax = __UINT64_C(255);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_UINT16:
        kind = 0;
        local_umin = __UINT64_C(0);
        local_umax = __UINT64_C(65535);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_UINT32:
        kind = 0;
        local_umin = __UINT64_C(0);
        local_umax = __UINT64_C(4294967295);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_UINT64:
        kind = 0;
        local_umin = __UINT64_C(0);
        local_umax = __UINT64_C(18446744073709551615);

        if (!str_restr && type->info.num.range) {
            str_restr = type->info.num.range->expr;
        }
        break;
    case LY_TYPE_STRING:
        kind = 0;
        local_umin = __UINT64_C(0);
        local_umax = __UINT64_C(18446744073709551615);

        if (!str_restr && type->info.str.length) {
            str_restr = type->info.str.length->expr;
        }
        break;
    default:
        return -1;
    }

    /* process superior types */
    if (type->der) {
        if (resolve_len_ran_interval(ctx, NULL, &type->der->type, &intv)) {
            return -1;
        }
        assert(!intv || (intv->kind == kind));
    }

    if (!str_restr) {
        /* we do not have any restriction, return superior ones */
        *ret = intv;
        return EXIT_SUCCESS;
    }

    /* adjust local min and max */
    if (intv) {
        tmp_intv = intv;

        if (kind == 0) {
            local_umin = tmp_intv->value.uval.min;
        } else if (kind == 1) {
            local_smin = tmp_intv->value.sval.min;
        } else if (kind == 2) {
            local_fmin = tmp_intv->value.fval.min;
        }

        while (tmp_intv->next) {
            tmp_intv = tmp_intv->next;
        }

        if (kind == 0) {
            local_umax = tmp_intv->value.uval.max;
        } else if (kind == 1) {
            local_smax = tmp_intv->value.sval.max;
        } else if (kind == 2) {
            local_fmax = tmp_intv->value.fval.max;
        }
    }

    /* finally parse our restriction */
    seg_ptr = str_restr;
    tmp_intv = NULL;
    while (1) {
        if (!tmp_local_intv) {
            assert(!local_intv);
            local_intv = malloc(sizeof *local_intv);
            tmp_local_intv = local_intv;
        } else {
            tmp_local_intv->next = malloc(sizeof *tmp_local_intv);
            tmp_local_intv = tmp_local_intv->next;
        }
        LY_CHECK_ERR_GOTO(!tmp_local_intv, LOGMEM(ctx), error);

        tmp_local_intv->kind = kind;
        tmp_local_intv->type = type;
        tmp_local_intv->next = NULL;

        /* min */
        ptr = seg_ptr;
        while (isspace(ptr[0])) {
            ++ptr;
        }
        if (isdigit(ptr[0]) || (ptr[0] == '+') || (ptr[0] == '-')) {
            if (kind == 0) {
                tmp_local_intv->value.uval.min = strtoull(ptr, (char **)&ptr, 10);
            } else if (kind == 1) {
                tmp_local_intv->value.sval.min = strtoll(ptr, (char **)&ptr, 10);
            } else if (kind == 2) {
                if (parse_range_dec64(&ptr, local_fdig, &tmp_local_intv->value.fval.min)) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, ptr, "range");
                    goto error;
                }
            }
        } else if (!strncmp(ptr, "min", 3)) {
            if (kind == 0) {
                tmp_local_intv->value.uval.min = local_umin;
            } else if (kind == 1) {
                tmp_local_intv->value.sval.min = local_smin;
            } else if (kind == 2) {
                tmp_local_intv->value.fval.min = local_fmin;
            }

            ptr += 3;
        } else if (!strncmp(ptr, "max", 3)) {
            if (kind == 0) {
                tmp_local_intv->value.uval.min = local_umax;
            } else if (kind == 1) {
                tmp_local_intv->value.sval.min = local_smax;
            } else if (kind == 2) {
                tmp_local_intv->value.fval.min = local_fmax;
            }

            ptr += 3;
        } else {
            goto error;
        }

        while (isspace(ptr[0])) {
            ptr++;
        }

        /* no interval or interval */
        if ((ptr[0] == '|') || !ptr[0]) {
            if (kind == 0) {
                tmp_local_intv->value.uval.max = tmp_local_intv->value.uval.min;
            } else if (kind == 1) {
                tmp_local_intv->value.sval.max = tmp_local_intv->value.sval.min;
            } else if (kind == 2) {
                tmp_local_intv->value.fval.max = tmp_local_intv->value.fval.min;
            }
        } else if (!strncmp(ptr, "..", 2)) {
            /* skip ".." */
            ptr += 2;
            while (isspace(ptr[0])) {
                ++ptr;
            }

            /* max */
            if (isdigit(ptr[0]) || (ptr[0] == '+') || (ptr[0] == '-')) {
                if (kind == 0) {
                    tmp_local_intv->value.uval.max = strtoull(ptr, (char **)&ptr, 10);
                } else if (kind == 1) {
                    tmp_local_intv->value.sval.max = strtoll(ptr, (char **)&ptr, 10);
                } else if (kind == 2) {
                    if (parse_range_dec64(&ptr, local_fdig, &tmp_local_intv->value.fval.max)) {
                        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, ptr, "range");
                        goto error;
                    }
                }
            } else if (!strncmp(ptr, "max", 3)) {
                if (kind == 0) {
                    tmp_local_intv->value.uval.max = local_umax;
                } else if (kind == 1) {
                    tmp_local_intv->value.sval.max = local_smax;
                } else if (kind == 2) {
                    tmp_local_intv->value.fval.max = local_fmax;
                }
            } else {
                goto error;
            }
        } else {
            goto error;
        }

        /* check min and max in correct order */
        if (kind == 0) {
            /* current segment */
            if (tmp_local_intv->value.uval.min > tmp_local_intv->value.uval.max) {
                goto error;
            }
            if (tmp_local_intv->value.uval.min < local_umin || tmp_local_intv->value.uval.max > local_umax) {
                goto error;
            }
            /* segments sholud be ascending order */
            if (tmp_intv && (tmp_intv->value.uval.max >= tmp_local_intv->value.uval.min)) {
                goto error;
            }
        } else if (kind == 1) {
            if (tmp_local_intv->value.sval.min > tmp_local_intv->value.sval.max) {
                goto error;
            }
            if (tmp_local_intv->value.sval.min < local_smin || tmp_local_intv->value.sval.max > local_smax) {
                goto error;
            }
            if (tmp_intv && (tmp_intv->value.sval.max >= tmp_local_intv->value.sval.min)) {
                goto error;
            }
        } else if (kind == 2) {
            if (tmp_local_intv->value.fval.min > tmp_local_intv->value.fval.max) {
                goto error;
            }
            if (tmp_local_intv->value.fval.min < local_fmin || tmp_local_intv->value.fval.max > local_fmax) {
                goto error;
            }
            if (tmp_intv && (tmp_intv->value.fval.max >= tmp_local_intv->value.fval.min)) {
                /* fraction-digits value is always the same (it cannot be changed in derived types) */
                goto error;
            }
        }

        /* next segment (next OR) */
        seg_ptr = strchr(seg_ptr, '|');
        if (!seg_ptr) {
            break;
        }
        seg_ptr++;
        tmp_intv = tmp_local_intv;
    }

    /* check local restrictions against superior ones */
    if (intv) {
        tmp_intv = intv;
        tmp_local_intv = local_intv;

        while (tmp_local_intv && tmp_intv) {
            /* reuse local variables */
            if (kind == 0) {
                local_umin = tmp_local_intv->value.uval.min;
                local_umax = tmp_local_intv->value.uval.max;

                /* it must be in this interval */
                if ((local_umin >= tmp_intv->value.uval.min) && (local_umin <= tmp_intv->value.uval.max)) {
                    /* this interval is covered, next one */
                    if (local_umax <= tmp_intv->value.uval.max) {
                        tmp_local_intv = tmp_local_intv->next;
                        continue;
                    /* ascending order of restrictions -> fail */
                    } else {
                        goto error;
                    }
                }
            } else if (kind == 1) {
                local_smin = tmp_local_intv->value.sval.min;
                local_smax = tmp_local_intv->value.sval.max;

                if ((local_smin >= tmp_intv->value.sval.min) && (local_smin <= tmp_intv->value.sval.max)) {
                    if (local_smax <= tmp_intv->value.sval.max) {
                        tmp_local_intv = tmp_local_intv->next;
                        continue;
                    } else {
                        goto error;
                    }
                }
            } else if (kind == 2) {
                local_fmin = tmp_local_intv->value.fval.min;
                local_fmax = tmp_local_intv->value.fval.max;

                 if ((dec64cmp(local_fmin, local_fdig, tmp_intv->value.fval.min, local_fdig) > -1)
                        && (dec64cmp(local_fmin, local_fdig, tmp_intv->value.fval.max, local_fdig) < 1)) {
                    if (dec64cmp(local_fmax, local_fdig, tmp_intv->value.fval.max, local_fdig) < 1) {
                        tmp_local_intv = tmp_local_intv->next;
                        continue;
                    } else {
                        goto error;
                    }
                }
            }

            tmp_intv = tmp_intv->next;
        }

        /* some interval left uncovered -> fail */
        if (tmp_local_intv) {
            goto error;
        }
    }

    /* append the local intervals to all the intervals of the superior types, return it all */
    if (intv) {
        for (tmp_intv = intv; tmp_intv->next; tmp_intv = tmp_intv->next);
        tmp_intv->next = local_intv;
    } else {
        intv = local_intv;
    }
    *ret = intv;

    return EXIT_SUCCESS;

error:
    while (intv) {
        tmp_intv = intv->next;
        free(intv);
        intv = tmp_intv;
    }
    while (local_intv) {
        tmp_local_intv = local_intv->next;
        free(local_intv);
        local_intv = tmp_local_intv;
    }

    return -1;
}