Button* button_new(Manager *m, const char *name) {
        Button *b;

        assert(m);
        assert(name);

        b = new0(Button, 1);
        if (!b)
                return NULL;

        b->name = strdup(name);
        if (!b->name)
                return mfree(b);

        if (hashmap_put(m->buttons, b->name, b) < 0) {
                free(b->name);
                return mfree(b);
        }

        b->manager = m;
        b->fd = -1;

        return b;
}