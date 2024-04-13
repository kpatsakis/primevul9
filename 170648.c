static int create_socket(struct sockaddr_un *addr)
{
    int fd = -1;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        DEBUG("Failed to create socket");
        return -1;
    }

    /* remove previous socket file */
    if (unlink(addr->sun_path) < 0) {
        if (errno != ENOENT)
            g_error("Couldn't remove previous socket file %s", addr->sun_path);
    }
    /* remove of previous socket file successful */
    else
        g_warning("removed previous socket file %s", addr->sun_path);

    if(bind(fd, (struct sockaddr *)addr, sizeof(*addr)) < 0)
    {
        DEBUG("Failed to bind to socket");
        close(fd);
        return -1;
    }
    if(listen(fd, 30) < 0)
    {
        DEBUG( "Failed to listen to socket" );
        close(fd);
        return -1;
    }
    return fd;
}