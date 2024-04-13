const char* menu_cache_item_get_file_dirname( MenuCacheItem* item )
{
    return item->file_dir ? item->file_dir->dir + 1 : NULL;
}