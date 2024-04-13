static void xfm_error(struct tree *xfm, const char *msg) {
    char *v = strdup(msg);
    char *l = strdup("error");

    if (l == NULL || v == NULL)
        return;
    tree_append(xfm, l, v);
}