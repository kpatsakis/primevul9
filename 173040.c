static int store_error(struct augeas *aug,
                       const char *filename, const char *path,
                       const char *status, int errnum,
                       const struct lns_error *err, const char *text) {
    struct tree *err_info = NULL, *finfo = NULL;
    char *fip = NULL;
    int r;
    int result = -1;

    if (filename != NULL) {
        r = pathjoin(&fip, 2, AUGEAS_META_FILES, filename);
    } else {
        r = pathjoin(&fip, 2, AUGEAS_META_TEXT, path);
    }
    ERR_NOMEM(r < 0, aug);

    finfo = tree_find_cr(aug, fip);
    ERR_BAIL(aug);

    if (status != NULL) {
        err_info = tree_child_cr(finfo, s_error);
        ERR_NOMEM(err_info == NULL, aug);

        r = tree_set_value(err_info, status);
        ERR_NOMEM(r < 0, aug);

        /* Errors from err_set are ignored on purpose. We try
         * to report as much as we can */
        if (err != NULL) {
            if (err->pos >= 0) {
                size_t line, ofs;
                err_set(aug, err_info, s_pos, "%d", err->pos);
                if (text != NULL) {
                    calc_line_ofs(text, err->pos, &line, &ofs);
                    err_set(aug, err_info, s_line, "%zd", line);
                    err_set(aug, err_info, s_char, "%zd", ofs);
                }
            }
            if (err->path != NULL) {
                err_set(aug, err_info, s_path, "%s%s", path, err->path);
            }
            if (err->lens != NULL) {
                char *fi = format_info(err->lens->info);
                if (fi != NULL) {
                    err_set(aug, err_info, s_lens, "%s", fi);
                    free(fi);
                }
            }
            err_set(aug, err_info, s_message, "%s", err->message);
        } else if (errnum != 0) {
            const char *msg = strerror(errnum);
            err_set(aug, err_info, s_message, "%s", msg);
        }
    } else {
        /* No error, nuke the error node if it exists */
        err_info = tree_child(finfo, s_error);
        if (err_info != NULL) {
            tree_unlink_children(aug, err_info);
            pathx_symtab_remove_descendants(aug->symtab, err_info);
            tree_unlink(err_info);
        }
    }

    tree_clean(finfo);
    result = 0;
 error:
    free(fip);
    return result;
}