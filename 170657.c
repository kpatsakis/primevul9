static void menu_cache_file_dir_unref(MenuCacheFileDir *file_dir)
{
    if (file_dir && g_atomic_int_dec_and_test(&file_dir->n_ref))
    {
        g_free(file_dir->dir);
        g_free(file_dir);
    }
}