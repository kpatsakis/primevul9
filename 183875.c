yang_check_enum(struct ly_ctx *ctx, struct yang_type *typ, struct lys_type_enum *enm, int64_t *value, int assign)
{
    int i, j;

    if (!assign) {
        /* assign value automatically */
        if (*value > INT32_MAX) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, "2147483648", "enum/value");
            goto error;
        }
        enm->value = *value;
        enm->flags |= LYS_AUTOASSIGNED;
        (*value)++;
    } else if (typ->type->info.enums.enm == enm) {
        /* change value, which is assigned automatically, if first enum has value. */
        *value = typ->type->info.enums.enm[0].value;
        (*value)++;
    }

    /* check that the value is unique */
    j = typ->type->info.enums.count-1;
    for (i = 0; i < j; i++) {
        if (typ->type->info.enums.enm[i].value == typ->type->info.enums.enm[j].value) {
            LOGVAL(ctx, LYE_ENUM_DUPVAL, LY_VLOG_NONE, NULL,
                   typ->type->info.enums.enm[j].value, typ->type->info.enums.enm[j].name,
                   typ->type->info.enums.enm[i].name);
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}