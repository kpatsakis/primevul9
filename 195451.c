static int manager_setup_time_change(Manager *m) {
        int r;

        /* We only care for the cancellation event, hence we set the
         * timeout to the latest possible value. */
        struct itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        assert(m);
        assert_cc(sizeof(time_t) == sizeof(TIME_T_MAX));

        if (m->test_run)
                return 0;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever
         * CLOCK_REALTIME makes a jump relative to CLOCK_MONOTONIC */

        m->time_change_fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        if (m->time_change_fd < 0)
                return log_error_errno(errno, "Failed to create timerfd: %m");

        if (timerfd_settime(m->time_change_fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, NULL) < 0) {
                log_debug_errno(errno, "Failed to set up TFD_TIMER_CANCEL_ON_SET, ignoring: %m");
                m->time_change_fd = safe_close(m->time_change_fd);
                return 0;
        }

        r = sd_event_add_io(m->event, &m->time_change_event_source, m->time_change_fd, EPOLLIN, manager_dispatch_time_change_fd, m);
        if (r < 0)
                return log_error_errno(r, "Failed to create time change event source: %m");

        log_debug("Set up TFD_TIMER_CANCEL_ON_SET timerfd.");

        return 0;
}