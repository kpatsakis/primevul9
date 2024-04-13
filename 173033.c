void transform_file_error(struct augeas *aug, const char *status,
                          const char *filename, const char *format, ...) {
    char *ep = err_path(filename);
    struct tree *err;
    char *msg;
    va_list ap;
    int r;

    err = tree_find_cr(aug, ep);
    if (err == NULL)
        return;

    tree_unlink_children(aug, err);
    tree_set_value(err, status);

    err = tree_child_cr(err, s_message);
    if (err == NULL)
        return;

    va_start(ap, format);
    r = vasprintf(&msg, format, ap);
    va_end(ap);
    if (r < 0)
        return;
    tree_set_value(err, msg);
    free(msg);
}