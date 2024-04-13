dup_identities_check(const char *id, struct lys_module *module)
{
    struct lys_module *mainmod;
    int i;

    if (dup_identity_check(id, module->ident, module->ident_size)) {
        LOGVAL(module->ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "identity", id);
        return EXIT_FAILURE;
    }

    /* check identity in submodules */
    mainmod = lys_main_module(module);
    for (i = 0; i < mainmod->inc_size && mainmod->inc[i].submodule; ++i) {
        if (dup_identity_check(id, mainmod->inc[i].submodule->ident, mainmod->inc[i].submodule->ident_size)) {
            LOGVAL(module->ctx, LYE_DUPID, LY_VLOG_NONE, NULL, "identity", id);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}