lyp_check_includedup(struct lys_module *mod, const char *name, struct lys_include *inc, struct lys_submodule **sub)
{
    struct lys_module **parsed_sub = mod->ctx->models.parsed_submodules;
    uint8_t i, parsed_sub_count = mod->ctx->models.parsed_submodules_count;

    assert(sub);

    for (i = 0; i < mod->inc_size; ++i) {
        if (ly_strequal(mod->inc[i].submodule->name, name, 1)) {
            /* the same module is already included in the same module - error */
            LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, name, "include");
            LOGVAL(mod->ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Submodule \"%s\" included twice in the same module \"%s\".",
                   name, mod->name);
            return -1;
        }
    }

    if (parsed_sub_count) {
        assert(!parsed_sub[0]->type);
        for (i = parsed_sub_count - 1; parsed_sub[i]->type; --i) {
            if (ly_strequal(parsed_sub[i]->name, name, 1)) {
                /* check revisions, including multiple revisions of a single module is error */
                if (inc->rev[0] && (!parsed_sub[i]->rev_size || strcmp(parsed_sub[i]->rev[0].date, inc->rev))) {
                    /* the already included submodule has
                     * - no revision, but here we require some
                     * - different revision than the one required here */
                    LOGVAL(mod->ctx, LYE_INARG, LY_VLOG_NONE, NULL, name, "include");
                    LOGVAL(mod->ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Including multiple revisions of submodule \"%s\".", name);
                    return -1;
                }

                /* the same module is already included in some other submodule, return it */
                (*sub) = (struct lys_submodule *)parsed_sub[i];
                return 1;
            }
        }
    }

    /* no duplicity found */
    return 0;
}