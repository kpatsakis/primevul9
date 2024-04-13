void v9fs_path_free(V9fsPath *path)
{
    g_free(path->data);
    path->data = NULL;
    path->size = 0;
}