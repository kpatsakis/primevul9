static bool file_current(struct augeas *aug, const char *fname,
                         struct tree *finfo) {
    struct tree *mtime = tree_child(finfo, s_mtime);
    struct tree *file = NULL, *path = NULL;
    int r;
    struct stat st;
    int64_t mtime_i;

    if (mtime == NULL || mtime->value == NULL)
        return false;

    r = xstrtoint64(mtime->value, 10, &mtime_i);
    if (r < 0) {
        /* Ignore silently and err on the side of caution */
        return false;
    }

    r = stat(fname, &st);
    if (r < 0)
        return false;

    if (mtime_i != (int64_t) st.st_mtime)
        return false;

    path = tree_child(finfo, s_path);
    if (path == NULL)
        return false;

    file = tree_find(aug, path->value);
    return (file != NULL && ! file->dirty);
}