static gboolean delayed_reload( Cache* cache )
{
    if(g_source_is_destroyed(g_main_current_source()))
        return FALSE;

    if(cache->need_reload)
        do_reload(cache);

    if (cache->need_reload)
      return TRUE;

    cache->delayed_reload_handler = 0;
    return FALSE;
}