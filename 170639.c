gboolean menu_cache_dir_is_visible(MenuCacheDir *dir)
{
    return ((dir->flags & FLAG_IS_NODISPLAY) == 0);
}