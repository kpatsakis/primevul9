gboolean menu_cache_app_get_use_sn( MenuCacheApp* app )
{
    return ( (app->flags & FLAG_USE_SN) != 0 );
}