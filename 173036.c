static int load_file(struct augeas *aug, struct lens *lens,
                     const char *lens_name, char *filename) {
    char *text = NULL;
    const char *err_status = NULL;
    struct tree *tree = NULL;
    char *path = NULL;
    struct lns_error *err = NULL;
    struct span *span = NULL;
    int result = -1, r, text_len = 0;

    path = file_name_path(aug, filename);
    ERR_NOMEM(path == NULL, aug);

    r = add_file_info(aug, path, lens, lens_name, filename, false);
    if (r < 0)
        goto done;

    text = xread_file(filename);
    if (text == NULL) {
        err_status = "read_failed";
        goto done;
    }
    text_len = strlen(text);
    text = append_newline(text, text_len);

    struct info *info;
    make_ref(info);
    make_ref(info->filename);
    info->filename->str = strdup(filename);
    info->error = aug->error;
    info->flags = aug->flags;
    info->first_line = 1;

    if (aug->flags & AUG_ENABLE_SPAN) {
        span = make_span(info);
        ERR_NOMEM(span == NULL, info);
    }

    tree = lns_get(info, lens, text, &err);

    unref(info, info);

    if (err != NULL) {
        err_status = "parse_failed";
        goto done;
    }

    tree_replace(aug, path, tree);

    /* top level node span entire file length */
    if (span != NULL && tree != NULL) {
        tree->parent->span = span;
        tree->parent->span->span_start = 0;
        tree->parent->span->span_end = text_len;
    }

    tree = NULL;

    result = 0;
 done:
    store_error(aug, filename + strlen(aug->root) - 1, path, err_status,
                errno, err, text);
 error:
    free_lns_error(err);
    free(path);
    free_tree(tree);
    free(text);
    return result;
}