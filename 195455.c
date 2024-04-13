static unsigned manager_dispatch_gc_queue(Manager *m) {
        Unit *u;
        unsigned n = 0;
        unsigned gc_marker;

        assert(m);

        /* log_debug("Running GC..."); */

        m->gc_marker += _GC_OFFSET_MAX;
        if (m->gc_marker + _GC_OFFSET_MAX <= _GC_OFFSET_MAX)
                m->gc_marker = 1;

        gc_marker = m->gc_marker;

        while ((u = m->gc_queue)) {
                assert(u->in_gc_queue);

                unit_gc_sweep(u, gc_marker);

                LIST_REMOVE(gc_queue, m->gc_queue, u);
                u->in_gc_queue = false;

                n++;

                if (u->gc_marker == gc_marker + GC_OFFSET_BAD ||
                    u->gc_marker == gc_marker + GC_OFFSET_UNSURE) {
                        log_unit_debug(u->id, "Collecting %s", u->id);
                        u->gc_marker = gc_marker + GC_OFFSET_BAD;
                        unit_add_to_cleanup_queue(u);
                }
        }

        m->n_in_gc_queue = 0;

        return n;
}