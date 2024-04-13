static int add_file_info(struct augeas *aug, const char *node,
                         struct lens *lens, const char *lens_name,
                         const char *filename, bool force_reload) {
    struct tree *file, *tree;
    char *tmp = NULL;
    int r;
    char *path = NULL;
    int result = -1;

    if (lens == NULL)
        return -1;

    r = pathjoin(&path, 2, AUGEAS_META_TREE, node);
    ERR_NOMEM(r < 0, aug);

    file = tree_find_cr(aug, path);
    ERR_BAIL(aug);

    /* Set 'path' */
    tree = tree_child_cr(file, s_path);
    ERR_NOMEM(tree == NULL, aug);
    r = tree_set_value(tree, node);
    ERR_NOMEM(r < 0, aug);

    /* Set 'mtime' */
    if (force_reload) {
        tmp = strdup("0");
        ERR_NOMEM(tmp == NULL, aug);
    } else {
        tmp = mtime_as_string(aug, filename);
        ERR_BAIL(aug);
    }
    tree = tree_child_cr(file, s_mtime);
    ERR_NOMEM(tree == NULL, aug);
    tree_store_value(tree, &tmp);

    /* Set 'lens/info' */
    tmp = format_info(lens->info);
    ERR_NOMEM(tmp == NULL, aug);
    tree = tree_path_cr(file, 2, s_lens, s_info);
    ERR_NOMEM(tree == NULL, aug);
    r = tree_set_value(tree, tmp);
    ERR_NOMEM(r < 0, aug);
    FREE(tmp);

    /* Set 'lens' */
    tree = tree->parent;
    r = tree_set_value(tree, lens_name);
    ERR_NOMEM(r < 0, aug);

    tree_clean(file);

    result = 0;
 error:
    free(path);
    free(tmp);
    return result;
}