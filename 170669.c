static gboolean register_menu_to_server(MenuCache* cache)
{
    ssize_t len = strlen(cache->reg);
    /* FIXME: do unblocking I/O */
    if(write(server_fd, cache->reg, len) < len)
    {
        DEBUG("register_menu_to_server: sending failed");
        return FALSE; /* socket write failed */
    }
    return TRUE;
}