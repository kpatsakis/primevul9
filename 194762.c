hook_connect (struct t_weechat_plugin *plugin, const char *proxy,
              const char *address, int port, int ipv6, int retry,
              void *gnutls_sess, void *gnutls_cb, int gnutls_dhkey_size,
              const char *gnutls_priorities, const char *local_hostname,
              t_hook_callback_connect *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_connect *new_hook_connect;
#ifdef HOOK_CONNECT_MAX_SOCKETS
    int i;
#endif

#ifndef HAVE_GNUTLS
    /* make C compiler happy */
    (void) gnutls_sess;
    (void) gnutls_cb;
    (void) gnutls_dhkey_size;
    (void) gnutls_priorities;
#endif

    if (!address || (port <= 0) || !callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_connect = malloc (sizeof (*new_hook_connect));
    if (!new_hook_connect)
    {
        free (new_hook);
        return NULL;
    }

    hook_init_data (new_hook, plugin, HOOK_TYPE_CONNECT, HOOK_PRIORITY_DEFAULT,
                    callback_data);

    new_hook->hook_data = new_hook_connect;
    new_hook_connect->callback = callback;
    new_hook_connect->proxy = (proxy) ? strdup (proxy) : NULL;
    new_hook_connect->address = strdup (address);
    new_hook_connect->port = port;
    new_hook_connect->sock = -1;
    new_hook_connect->ipv6 = ipv6;
    new_hook_connect->retry = retry;
#ifdef HAVE_GNUTLS
    new_hook_connect->gnutls_sess = gnutls_sess;
    new_hook_connect->gnutls_cb = gnutls_cb;
    new_hook_connect->gnutls_dhkey_size = gnutls_dhkey_size;
    new_hook_connect->gnutls_priorities = (gnutls_priorities) ?
        strdup (gnutls_priorities) : NULL;
#endif
    new_hook_connect->local_hostname = (local_hostname) ?
        strdup (local_hostname) : NULL;
    new_hook_connect->child_read = -1;
    new_hook_connect->child_write = -1;
    new_hook_connect->child_recv = -1;
    new_hook_connect->child_send = -1;
    new_hook_connect->child_pid = 0;
    new_hook_connect->hook_child_timer = NULL;
    new_hook_connect->hook_fd = NULL;
    new_hook_connect->handshake_hook_fd = NULL;
    new_hook_connect->handshake_hook_timer = NULL;
    new_hook_connect->handshake_fd_flags = 0;
    new_hook_connect->handshake_ip_address = NULL;
#ifdef HOOK_CONNECT_MAX_SOCKETS
    for (i = 0; i < HOOK_CONNECT_MAX_SOCKETS; i++)
    {
        new_hook_connect->sock_v4[i] = -1;
        new_hook_connect->sock_v6[i] = -1;
    }
#endif

    hook_add_to_list (new_hook);

    network_connect_with_fork (new_hook);

    return new_hook;
}