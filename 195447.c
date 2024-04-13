static int manager_setup_notify(Manager *m) {
        int r;

        if (m->test_run)
                return 0;

        if (m->notify_fd < 0) {
                _cleanup_close_ int fd = -1;
                union sockaddr_union sa =  {
                        .sa.sa_family = AF_UNIX,
                };
                static const int one = 1;

                /* First free all secondary fields */
                free(m->notify_socket);
                m->notify_socket = NULL;
                m->notify_event_source = sd_event_source_unref(m->notify_event_source);

                fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
                if (fd < 0)
                        return log_error_errno(errno, "Failed to allocate notification socket: %m");

                if (m->running_as == SYSTEMD_SYSTEM)
                        m->notify_socket = strdup("/run/systemd/notify");
                else {
                        const char *e;

                        e = getenv("XDG_RUNTIME_DIR");
                        if (!e) {
                                log_error_errno(errno, "XDG_RUNTIME_DIR is not set: %m");
                                return -EINVAL;
                        }

                        m->notify_socket = strappend(e, "/systemd/notify");
                }
                if (!m->notify_socket)
                        return log_oom();

                (void) mkdir_parents_label(m->notify_socket, 0755);
                (void) unlink(m->notify_socket);

                strncpy(sa.un.sun_path, m->notify_socket, sizeof(sa.un.sun_path)-1);
                r = bind(fd, &sa.sa, offsetof(struct sockaddr_un, sun_path) + strlen(sa.un.sun_path));
                if (r < 0)
                        return log_error_errno(errno, "bind(%s) failed: %m", sa.un.sun_path);

                r = setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &one, sizeof(one));
                if (r < 0)
                        return log_error_errno(errno, "SO_PASSCRED failed: %m");

                m->notify_fd = fd;
                fd = -1;

                log_debug("Using notification socket %s", m->notify_socket);
        }

        if (!m->notify_event_source) {
                r = sd_event_add_io(m->event, &m->notify_event_source, m->notify_fd, EPOLLIN, manager_dispatch_notify_fd, m);
                if (r < 0)
                        return log_error_errno(r, "Failed to allocate notify event source: %m");

                /* Process signals a bit earlier than SIGCHLD, so that we can
                 * still identify to which service an exit message belongs */
                r = sd_event_source_set_priority(m->notify_event_source, -7);
                if (r < 0)
                        return log_error_errno(r, "Failed to set priority of notify event source: %m");
        }

        return 0;
}