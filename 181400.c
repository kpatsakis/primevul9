void v9fs_path_copy(V9fsPath *dst, const V9fsPath *src)
{
    v9fs_path_free(dst);
    dst->size = src->size;
    dst->data = g_memdup(src->data, src->size);
}