gboolean menu_cache_app_get_use_terminal( MenuCacheApp* app )
{
    return ( (app->flags & FLAG_USE_TERMINAL) != 0 );
}