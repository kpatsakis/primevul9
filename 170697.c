static void unregister_menu_from_server( MenuCache* cache )
{
    char buf[38];
    g_snprintf( buf, 38, "UNR:%s\n", cache->md5 );
    /* FIXME: do unblocking I/O */
    if(write( server_fd, buf, 37 ) <= 0)
    {
        DEBUG("unregister_menu_from_server: sending failed");
    }
}