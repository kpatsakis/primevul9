static gboolean menu_cache_reload_idle(gpointer cache)
{
    /* do reload once */
    if (!g_source_is_destroyed(g_main_current_source()))
        menu_cache_reload(cache);
    return FALSE;
}