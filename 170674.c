char* menu_cache_item_get_file_path( MenuCacheItem* item )
{
    if( ! item->file_name || ! item->file_dir )
        return NULL;
    return g_build_filename( item->file_dir->dir + 1, item->file_name, NULL );
}