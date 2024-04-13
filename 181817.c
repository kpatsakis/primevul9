lyp_ctx_check_module(struct lys_module *module)
{
    struct ly_ctx *ctx;
    int i, match_i = -1, to_implement;
    const char *last_rev = NULL;

    assert(module);
    to_implement = 0;
    ctx = module->ctx;

    /* find latest revision */
    for (i = 0; i < module->rev_size; ++i) {
        if (!last_rev || (strcmp(last_rev, module->rev[i].date) < 0)) {
            last_rev = module->rev[i].date;
        }
    }

    for (i = 0; i < ctx->models.used; i++) {
        /* check name (name/revision) and namespace uniqueness */
        if (!strcmp(ctx->models.list[i]->name, module->name)) {
            if (to_implement) {
                if (i == match_i) {
                    continue;
                }
                LOGERR(ctx, LY_EINVAL, "Module \"%s@%s\" in another revision \"%s\" already implemented.",
                       module->name, last_rev ? last_rev : "<latest>", ctx->models.list[i]->rev[0].date);
                return -1;
            } else if (!ctx->models.list[i]->rev_size && module->rev_size) {
                LOGERR(ctx, LY_EINVAL, "Module \"%s\" without revision already in context.", module->name);
                return -1;
            } else if (ctx->models.list[i]->rev_size && !module->rev_size) {
                LOGERR(ctx, LY_EINVAL, "Module \"%s\" with revision \"%s\" already in context.",
                       module->name, ctx->models.list[i]->rev[0].date);
                return -1;
            } else if ((!module->rev_size && !ctx->models.list[i]->rev_size)
                    || !strcmp(ctx->models.list[i]->rev[0].date, last_rev)) {

                LOGVRB("Module \"%s@%s\" already in context.", module->name, last_rev ? last_rev : "<latest>");

                /* if disabled, enable first */
                if (ctx->models.list[i]->disabled) {
                    lys_set_enabled(ctx->models.list[i]);
                }

                to_implement = module->implemented;
                match_i = i;
                if (to_implement && !ctx->models.list[i]->implemented) {
                    /* check first that it is okay to change it to implemented */
                    i = -1;
                    continue;
                }
                return 1;

            } else if (module->implemented && ctx->models.list[i]->implemented) {
                LOGERR(ctx, LY_EINVAL, "Module \"%s@%s\" in another revision \"%s\" already implemented.",
                       module->name, last_rev ? last_rev : "<latest>", ctx->models.list[i]->rev[0].date);
                return -1;
            }
            /* else keep searching, for now the caller is just adding
             * another revision of an already present schema
             */
        } else if (!strcmp(ctx->models.list[i]->ns, module->ns)) {
            LOGERR(ctx, LY_EINVAL, "Two different modules (\"%s\" and \"%s\") have the same namespace \"%s\".",
                   ctx->models.list[i]->name, module->name, module->ns);
            return -1;
        }
    }

    if (to_implement) {
        if (lys_set_implemented(ctx->models.list[match_i])) {
            return -1;
        }
        return 1;
    }

    return 0;
}