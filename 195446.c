int manager_startup(Manager *m, FILE *serialization, FDSet *fds) {
        int r, q;

        assert(m);

        dual_timestamp_get(&m->generators_start_timestamp);
        r = manager_run_generators(m);
        dual_timestamp_get(&m->generators_finish_timestamp);
        if (r < 0)
                return r;

        r = lookup_paths_init(
                        &m->lookup_paths, m->running_as, true,
                        NULL,
                        m->generator_unit_path,
                        m->generator_unit_path_early,
                        m->generator_unit_path_late);
        if (r < 0)
                return r;

        manager_build_unit_path_cache(m);

        /* If we will deserialize make sure that during enumeration
         * this is already known, so we increase the counter here
         * already */
        if (serialization)
                m->n_reloading ++;

        /* First, enumerate what we can from all config files */
        dual_timestamp_get(&m->units_load_start_timestamp);
        r = manager_enumerate(m);
        dual_timestamp_get(&m->units_load_finish_timestamp);

        /* Second, deserialize if there is something to deserialize */
        if (serialization)
                r = manager_deserialize(m, serialization, fds);

        /* Any fds left? Find some unit which wants them. This is
         * useful to allow container managers to pass some file
         * descriptors to us pre-initialized. This enables
         * socket-based activation of entire containers. */
        if (fdset_size(fds) > 0) {
                q = manager_distribute_fds(m, fds);
                if (q < 0 && r == 0)
                        r = q;
        }

        /* We might have deserialized the notify fd, but if we didn't
         * then let's create the bus now */
        q = manager_setup_notify(m);
        if (q < 0 && r == 0)
                r = q;

        /* We might have deserialized the kdbus control fd, but if we
         * didn't, then let's create the bus now. */
        manager_setup_kdbus(m);
        manager_connect_bus(m, !!serialization);
        bus_track_coldplug(m, &m->subscribed, &m->deserialized_subscribed);

        /* Third, fire things up! */
        q = manager_coldplug(m);
        if (q < 0 && r == 0)
                r = q;

        if (serialization) {
                assert(m->n_reloading > 0);
                m->n_reloading --;

                /* Let's wait for the UnitNew/JobNew messages being
                 * sent, before we notify that the reload is
                 * finished */
                m->send_reloading_done = true;
        }

        return r;
}