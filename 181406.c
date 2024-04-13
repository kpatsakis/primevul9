static int v9fs_path_is_ancestor(V9fsPath *s1, V9fsPath *s2)
{
    if (!strncmp(s1->data, s2->data, s1->size - 1)) {
        if (s2->data[s1->size - 1] == '\0' || s2->data[s1->size - 1] == '/') {
            return 1;
        }
    }
    return 0;
}