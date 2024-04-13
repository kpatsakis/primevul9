int manager_enumerate(Manager *m) {
        int r = 0;
        UnitType c;

        assert(m);

        /* Let's ask every type to load all units from disk/kernel
         * that it might know */
        for (c = 0; c < _UNIT_TYPE_MAX; c++) {
                int q;

                if (unit_vtable[c]->supported && !unit_vtable[c]->supported(m)) {
                        log_info("Unit type .%s is not supported on this system.", unit_type_to_string(c));
                        continue;
                }

                if (!unit_vtable[c]->enumerate)
                        continue;

                q = unit_vtable[c]->enumerate(m);
                if (q < 0)
                        r = q;
        }

        manager_dispatch_load_queue(m);
        return r;
}