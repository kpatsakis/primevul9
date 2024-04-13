int main(int argc, char** argv)
{
    GIOChannel* ch;
    int server_fd;
    struct sockaddr_un addr;
#ifndef DISABLE_DAEMONIZE
    int fd, pid;

    long open_max;
    long i;
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (argc < 2)
        /* old way, not recommended! */
        get_socket_name( addr.sun_path, sizeof( addr.sun_path ) );
    else if (strlen(argv[1]) >= sizeof(addr.sun_path))
        /* ouch, it's too big! */
        return 1;
    else
        /* this is a good way! */
        strncpy(addr.sun_path, argv[1], sizeof(addr.sun_path) - 1);

    socket_file = addr.sun_path;

    server_fd = create_socket(&addr);

    if( server_fd < 0 )
        return 1;

#ifndef DISABLE_DAEMONIZE
    /* Become a daemon */
    if ((pid = fork()) < 0) {
        g_error("can't fork");
    }
    else if (pid != 0) {
        /* exit parent */
        exit(0);
    }

    /* reset session to forget about parent process completely */
    setsid();

    /* change working directory to root, so previous working directory
     * can be unmounted */
    if (chdir("/") < 0) {
        g_error("can't change directory to /");
    }

    open_max = sysconf (_SC_OPEN_MAX);
    for (i = 0; i < open_max; i++)
    {
        /* don't hold open fd opened besides server socket */
        if (i != server_fd)
            fcntl (i, F_SETFD, FD_CLOEXEC);
    }

    /* /dev/null for stdin, stdout, stderr */
    fd = open ("/dev/null", O_RDONLY);
    if (fd != -1)
    {
        dup2 (fd, 0);
        close (fd);
    }
    fd = open ("/dev/null", O_WRONLY);
    if (fd != -1)
    {
        dup2 (fd, 1);
        dup2 (fd, 2);
        close (fd);
    }
#endif

    signal(SIGHUP, terminate);
    signal(SIGINT, terminate);
    signal(SIGQUIT, terminate);
    signal(SIGTERM, terminate);
    signal(SIGPIPE, SIG_IGN);

    ch = g_io_channel_unix_new(server_fd);
    if(!ch)
        return 1;
    g_io_add_watch(ch, G_IO_IN|G_IO_PRI, on_new_conn_incoming, NULL);
    g_io_add_watch(ch, G_IO_ERR, on_server_conn_close, NULL);
    g_io_channel_unref(ch);

#if !GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init();
#endif

    hash = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, NULL);

    main_loop = g_main_loop_new( NULL, TRUE );
    g_main_loop_run( main_loop );
    g_main_loop_unref( main_loop );

    unlink(addr.sun_path);

    g_hash_table_destroy(hash);
    return 0;
}