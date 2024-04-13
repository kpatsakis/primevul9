static void v9fs_fix_path(V9fsPath *dst, V9fsPath *src, int len)
{
    V9fsPath str;
    v9fs_path_init(&str);
    v9fs_path_copy(&str, dst);
    v9fs_path_sprintf(dst, "%s%s", src->data, str.data + len);
    v9fs_path_free(&str);
}