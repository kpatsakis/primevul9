static gpointer server_io_thread(gpointer data)
{
    char buf[1024]; /* protocol has a lot shorter strings */
    ssize_t sz;
    size_t ptr = 0;
    int fd = GPOINTER_TO_INT(data);
    GHashTableIter it;
    char* menu_name;
    MenuCache* cache;

    while(fd >= 0)
    {
        sz = read(fd, &buf[ptr], sizeof(buf) - ptr);
        if(sz <= 0) /* socket error or EOF */
        {
            MENU_CACHE_LOCK;
            ptr = hash ? g_hash_table_size(hash) : 0;
            MENU_CACHE_UNLOCK;
            if (ptr == 0) /* don't need it anymore */
                break;
            G_LOCK(connect);
            if(fd != server_fd) /* someone replaced us?! go out immediately! */
            {
                G_UNLOCK(connect);
                break;
            }
            server_fd = -1;
            G_UNLOCK(connect);
            DEBUG("connect failed, trying reconnect");
            sleep(1);
            if( ! connect_server(NULL) )
            {
                g_critical("fail to re-connect to the server.");
                MENU_CACHE_LOCK;
                if(hash)
                {
                    g_hash_table_iter_init(&it, hash);
                    while(g_hash_table_iter_next(&it, (gpointer*)&menu_name, (gpointer*)&cache))
                        SET_CACHE_READY(cache);
                }
                MENU_CACHE_UNLOCK;
                break;
            }
            DEBUG("successfully reconnected server, re-register menus.");
            /* re-register all menu caches */
            MENU_CACHE_LOCK;
            if(hash)
            {
                g_hash_table_iter_init(&it, hash);
                while(g_hash_table_iter_next(&it, (gpointer*)&menu_name, (gpointer*)&cache))
                    register_menu_to_server(cache);
                    /* FIXME: need we remove it from hash if failed? */
            }
            MENU_CACHE_UNLOCK;
            break; /* next thread will do it */
        }
        while(sz > 0)
        {
            while(sz > 0)
            {
                if(buf[ptr] == '\n')
                    break;
                sz--;
                ptr++;
            }
            if(ptr == sizeof(buf)) /* EOB reached, seems we got garbage */
            {
                g_warning("menu cache: got garbage from server, break connect");
                shutdown(fd, SHUT_RDWR); /* drop connection */
                break; /* we handle it above */
            }
            else if(sz == 0) /* incomplete line, wait for data again */
                break;
            /* we got a line, let check what we got */
            buf[ptr] = '\0';
            if(memcmp(buf, "REL:", 4) == 0) /* reload */
            {
                DEBUG("server ask us to reload cache: %s", &buf[4]);
                MENU_CACHE_LOCK;
                if(hash)
                {
                    g_hash_table_iter_init(&it, hash);
                    while(g_hash_table_iter_next(&it, (gpointer*)&menu_name, (gpointer*)&cache))
                    {
                        if(memcmp(cache->md5, &buf[4], 32) == 0)
                        {
                            DEBUG("RELOAD!");
                            menu_cache_reload(cache);
                            SET_CACHE_READY(cache);
                            break;
                        }
                    }
                }
                MENU_CACHE_UNLOCK;
                /* DEBUG("cache reloaded"); */
            }
            else
                g_warning("menu cache: unrecognized input: %s", buf);
            /* go to next line */
            sz--;
            if(sz > 0)
                memmove(buf, &buf[ptr+1], sz);
            ptr = 0;
        }
    }
    G_LOCK(connect);
    if (fd == server_fd)
        server_fd = -1;
    G_UNLOCK(connect);
    close(fd);
    /* DEBUG("server io thread terminated"); */
#if GLIB_CHECK_VERSION(2, 32, 0)
    g_thread_unref(g_thread_self());
#endif
    return NULL;
}