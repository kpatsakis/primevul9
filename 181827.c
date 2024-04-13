lyp_get_import_module_ns(const struct lys_module *module, const char *ns)
{
    int i;
    const struct lys_module *mod = NULL;

    assert(module && ns);

    if (module->type) {
        /* the module is actually submodule and to get the namespace, we need the main module */
        if (ly_strequal(((struct lys_submodule *)module)->belongsto->ns, ns, 0)) {
            return ((struct lys_submodule *)module)->belongsto;
        }
    } else {
        /* module's own namespace */
        if (ly_strequal(module->ns, ns, 0)) {
            return module;
        }
    }

    /* imported modules */
    for (i = 0; i < module->imp_size; ++i) {
        if (ly_strequal(module->imp[i].module->ns, ns, 0)) {
            return module->imp[i].module;
        }
    }

    return mod;
}