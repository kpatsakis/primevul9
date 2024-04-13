v9fs_path_sprintf(V9fsPath *path, const char *fmt, ...)
{
    va_list ap;

    v9fs_path_free(path);

    va_start(ap, fmt);
    /* Bump the size for including terminating NULL */
    path->size = g_vasprintf(&path->data, fmt, ap) + 1;
    va_end(ap);
}