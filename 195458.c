static int manager_watch_jobs_in_progress(Manager *m) {
        usec_t next;

        assert(m);

        if (m->jobs_in_progress_event_source)
                return 0;

        next = now(CLOCK_MONOTONIC) + JOBS_IN_PROGRESS_WAIT_USEC;
        return sd_event_add_time(
                        m->event,
                        &m->jobs_in_progress_event_source,
                        CLOCK_MONOTONIC,
                        next, 0,
                        manager_dispatch_jobs_in_progress, m);
}