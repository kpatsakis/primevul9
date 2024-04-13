lyp_deviate_del_ext(struct lys_node *target, struct lys_ext_instance *ext)
{
    int n = -1, found = 0;
    char *path;

    while ((n = lys_ext_iter(target->ext, target->ext_size, n + 1, ext->insubstmt)) != -1) {
        if (target->ext[n]->def != ext->def) {
            continue;
        }

        if (ext->def->argument) {
            /* check matching arguments */
            if (!ly_strequal(target->ext[n]->arg_value, ext->arg_value, 1)) {
                continue;
            }
        }

        /* we have the matching extension - remove it */
        ++found;
        lyp_ext_instance_rm(target->module->ctx, &target->ext, &target->ext_size, n);
        --n;
    }

    if (!found) {
        path = lys_path(target, LYS_PATH_FIRST_PREFIX);
        LOGERR(target->module->ctx, LY_EVALID, "Extension deviation: extension \"%s\" to delete not found in \"%s\".",
               ext->def->name, path)
        free(path);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}