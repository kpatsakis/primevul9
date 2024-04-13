int v9fs_name_to_path(V9fsState *s, V9fsPath *dirpath,
                      const char *name, V9fsPath *path)
{
    int err;
    err = s->ops->name_to_path(&s->ctx, dirpath, name, path);
    if (err < 0) {
        err = -errno;
    }
    return err;
}