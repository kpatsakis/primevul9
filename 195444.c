int manager_reload(Manager *m) {
        int r, q;
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_fdset_free_ FDSet *fds = NULL;

        assert(m);

        r = manager_open_serialization(m, &f);
        if (r < 0)
                return r;

        m->n_reloading ++;
        bus_manager_send_reloading(m, true);

        fds = fdset_new();
        if (!fds) {
                m->n_reloading --;
                return -ENOMEM;
        }

        r = manager_serialize(m, f, fds, false);
        if (r < 0) {
                m->n_reloading --;
                return r;
        }

        if (fseeko(f, 0, SEEK_SET) < 0) {
                m->n_reloading --;
                return -errno;
        }

        /* From here on there is no way back. */
        manager_clear_jobs_and_units(m);
        manager_undo_generators(m);
        lookup_paths_free(&m->lookup_paths);

        /* Find new unit paths */
        q = manager_run_generators(m);
        if (q < 0 && r >= 0)
                r = q;

        q = lookup_paths_init(
                        &m->lookup_paths, m->running_as, true,
                        NULL,
                        m->generator_unit_path,
                        m->generator_unit_path_early,
                        m->generator_unit_path_late);
        if (q < 0 && r >= 0)
                r = q;

        manager_build_unit_path_cache(m);

        /* First, enumerate what we can from all config files */
        q = manager_enumerate(m);
        if (q < 0 && r >= 0)
                r = q;

        /* Second, deserialize our stored data */
        q = manager_deserialize(m, f, fds);
        if (q < 0 && r >= 0)
                r = q;

        fclose(f);
        f = NULL;

        /* Re-register notify_fd as event source */
        q = manager_setup_notify(m);
        if (q < 0 && r >= 0)
                r = q;

        /* Third, fire things up! */
        q = manager_coldplug(m);
        if (q < 0 && r >= 0)
                r = q;

        assert(m->n_reloading > 0);
        m->n_reloading--;

        m->send_reloading_done = true;

        return r;
}