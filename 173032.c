static struct tree *file_info(struct augeas *aug, const char *fname) {
    char *path = NULL;
    struct tree *result = NULL;
    int r;

    r = pathjoin(&path, 2, AUGEAS_META_FILES, fname);
    ERR_NOMEM(r < 0, aug);

    result = tree_find(aug, path);
    ERR_BAIL(aug);
 error:
    free(path);
    return result;
}