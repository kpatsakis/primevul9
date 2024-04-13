int text_store(struct augeas *aug, const char *lens_path,
               const char *path, const char *text) {
    struct info *info = NULL;
    struct lns_error *err = NULL;
    struct tree *tree = NULL;
    struct span *span = NULL;
    int result = -1;
    const char *err_status = NULL;
    struct lens *lens = NULL;

    lens = lens_from_name(aug, lens_path);
    if (lens == NULL) {
        goto done;
    }

    make_ref(info);
    info->first_line = 1;
    info->last_line = 1;
    info->first_column = 1;
    info->last_column = strlen(text);

    tree = lns_get(info, lens, text, &err);
    if (err != NULL) {
        err_status = "parse_failed";
        goto done;
    }

    unref(info, info);

    tree_replace(aug, path, tree);

    /* top level node span entire file length */
    if (span != NULL && tree != NULL) {
        tree->parent->span = span;
        tree->parent->span->span_start = 0;
        tree->parent->span->span_end = strlen(text);
    }

    tree = NULL;

    result = 0;
done:
    store_error(aug, NULL, path, err_status, errno, err, text);
    free_tree(tree);
    free_lns_error(err);
    return result;
}