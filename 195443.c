static int manager_coldplug(Manager *m) {
        int r = 0;
        Iterator i;
        Unit *u;
        char *k;

        assert(m);

        /* Then, let's set up their initial state. */
        HASHMAP_FOREACH_KEY(u, k, m->units, i) {
                int q;

                /* ignore aliases */
                if (u->id != k)
                        continue;

                q = unit_coldplug(u);
                if (q < 0)
                        r = q;
        }

        return r;
}