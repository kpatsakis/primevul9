static int manager_check_ask_password(Manager *m) {
        int r;

        assert(m);

        if (!m->ask_password_event_source) {
                assert(m->ask_password_inotify_fd < 0);

                mkdir_p_label("/run/systemd/ask-password", 0755);

                m->ask_password_inotify_fd = inotify_init1(IN_NONBLOCK|IN_CLOEXEC);
                if (m->ask_password_inotify_fd < 0)
                        return log_error_errno(errno, "inotify_init1() failed: %m");

                if (inotify_add_watch(m->ask_password_inotify_fd, "/run/systemd/ask-password", IN_CREATE|IN_DELETE|IN_MOVE) < 0) {
                        log_error_errno(errno, "Failed to add watch on /run/systemd/ask-password: %m");
                        manager_close_ask_password(m);
                        return -errno;
                }

                r = sd_event_add_io(m->event, &m->ask_password_event_source,
                                    m->ask_password_inotify_fd, EPOLLIN,
                                    manager_dispatch_ask_password_fd, m);
                if (r < 0) {
                        log_error_errno(errno, "Failed to add event source for /run/systemd/ask-password: %m");
                        manager_close_ask_password(m);
                        return -errno;
                }

                /* Queries might have been added meanwhile... */
                manager_dispatch_ask_password_fd(m->ask_password_event_source,
                                                 m->ask_password_inotify_fd, EPOLLIN, m);
        }

        return m->have_ask_password;
}