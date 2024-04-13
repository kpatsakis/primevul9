gboolean menu_cache_app_get_is_visible( MenuCacheApp* app, guint32 de_flags )
{
    if(app->flags & FLAG_IS_NODISPLAY)
        return FALSE;
    return (!app->show_in_flags || (app->show_in_flags & de_flags)) &&
           _can_be_exec(app);
}