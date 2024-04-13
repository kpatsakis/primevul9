static void v9fs_free_dirents(struct V9fsDirEnt *e)
{
    struct V9fsDirEnt *next = NULL;

    for (; e; e = next) {
        next = e->next;
        g_free(e->dent);
        g_free(e->st);
        g_free(e);
    }
}