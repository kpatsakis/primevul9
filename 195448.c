static void manager_print_jobs_in_progress(Manager *m) {
        _cleanup_free_ char *job_of_n = NULL;
        Iterator i;
        Job *j;
        unsigned counter = 0, print_nr;
        char cylon[6 + CYLON_BUFFER_EXTRA + 1];
        unsigned cylon_pos;
        char time[FORMAT_TIMESPAN_MAX], limit[FORMAT_TIMESPAN_MAX] = "no limit";
        uint64_t x;

        assert(m);
        assert(m->n_running_jobs > 0);

        manager_flip_auto_status(m, true);

        print_nr = (m->jobs_in_progress_iteration / JOBS_IN_PROGRESS_PERIOD_DIVISOR) % m->n_running_jobs;

        HASHMAP_FOREACH(j, m->jobs, i)
                if (j->state == JOB_RUNNING && counter++ == print_nr)
                        break;

        /* m->n_running_jobs must be consistent with the contents of m->jobs,
         * so the above loop must have succeeded in finding j. */
        assert(counter == print_nr + 1);
        assert(j);

        cylon_pos = m->jobs_in_progress_iteration % 14;
        if (cylon_pos >= 8)
                cylon_pos = 14 - cylon_pos;
        draw_cylon(cylon, sizeof(cylon), 6, cylon_pos);

        m->jobs_in_progress_iteration++;

        if (m->n_running_jobs > 1)
                asprintf(&job_of_n, "(%u of %u) ", counter, m->n_running_jobs);

        format_timespan(time, sizeof(time), now(CLOCK_MONOTONIC) - j->begin_usec, 1*USEC_PER_SEC);
        if (job_get_timeout(j, &x) > 0)
                format_timespan(limit, sizeof(limit), x - j->begin_usec, 1*USEC_PER_SEC);

        manager_status_printf(m, STATUS_TYPE_EPHEMERAL, cylon,
                              "%sA %s job is running for %s (%s / %s)",
                              strempty(job_of_n),
                              job_type_to_string(j->type),
                              unit_description(j->unit),
                              time, limit);
}