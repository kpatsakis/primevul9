yang_read_enum(struct ly_ctx *ctx, struct yang_type *typ, struct lys_type_enum *enm, char *value)
{
    int i, j;

    typ->base = LY_TYPE_ENUM;
    if (!value[0]) {
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "enum name");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Enum name must not be empty.");
        free(value);
        goto error;
    }

    enm->name = lydict_insert_zc(ctx, value);

    /* the assigned name MUST NOT have any leading or trailing whitespace characters */
    if (isspace(enm->name[0]) || isspace(enm->name[strlen(enm->name) - 1])) {
        LOGVAL(ctx, LYE_ENUM_WS, LY_VLOG_NONE, NULL, enm->name);
        goto error;
    }

    j = typ->type->info.enums.count - 1;
    /* check the name uniqueness */
    for (i = 0; i < j; i++) {
        if (ly_strequal(typ->type->info.enums.enm[i].name, enm->name, 1)) {
            LOGVAL(ctx, LYE_ENUM_DUPNAME, LY_VLOG_NONE, NULL, enm->name);
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}