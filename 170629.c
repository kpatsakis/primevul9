static gboolean connect_server(GCancellable* cancellable)
{
    int fd, rc;
    struct sockaddr_un addr;
    int retries = 0;

    G_LOCK(connect);
    if(server_fd != -1 || (cancellable && g_cancellable_is_cancelled(cancellable)))
    {
        G_UNLOCK(connect);
        return TRUE;
    }

retry:
    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        g_print("Failed to create socket\n");
        G_UNLOCK(connect);
        return FALSE;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    get_socket_name( addr.sun_path, sizeof( addr.sun_path ) );

    if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        rc = errno;
        close(fd);
        if(cancellable && g_cancellable_is_cancelled(cancellable))
        {
            G_UNLOCK(connect);
            return TRUE;
        }
        if((rc == ECONNREFUSED || rc == ENOENT) && retries == 0)
        {
            DEBUG("no running server found, starting it");
            fork_server(addr.sun_path);
            ++retries;
            goto retry;
        }
        if(retries < MAX_RETRIES)
        {
            usleep(50000);
            ++retries;
            goto retry;
        }
        g_print("Unable to connect\n");
        G_UNLOCK(connect);
        return FALSE;
    }
    server_fd = fd;
    G_UNLOCK(connect);
#if GLIB_CHECK_VERSION(2, 32, 0)
    g_thread_new("menu-cache-io", server_io_thread, GINT_TO_POINTER(fd));
#else
    g_thread_create(server_io_thread, GINT_TO_POINTER(fd), FALSE, NULL);
#endif
    return TRUE;
}