void menu_cache_init(int flags)
{
#if !GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init();
#endif
}