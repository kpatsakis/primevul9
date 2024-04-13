static gboolean on_new_conn_incoming(GIOChannel* ch, GIOCondition cond, gpointer user_data)
{
    int server, client;
    socklen_t client_addrlen;
    struct sockaddr client_addr;
    GIOChannel* child;
    ClientIO* client_io;

    server = g_io_channel_unix_get_fd(ch);

    client_addrlen = sizeof(client_addr);
    client = accept(server, &client_addr, &client_addrlen);
    if( client == -1 )
    {
        DEBUG("accept error");
        return TRUE;
    }

    child = g_io_channel_unix_new(client);
    g_io_channel_set_close_on_unref( child, TRUE );

    client_io = g_new0 (ClientIO, 1);
    client_io->channel = child;
    client_io->source_id =
        g_io_add_watch(child, G_IO_PRI|G_IO_IN|G_IO_HUP|G_IO_ERR,
                        on_client_data_in, client_io);
    g_io_channel_unref(child);

    DEBUG("new client accepted: %p", child);
    return TRUE;
}