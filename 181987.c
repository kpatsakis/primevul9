void button_free(Button *b) {
        assert(b);

        hashmap_remove(b->manager->buttons, b->name);

        sd_event_source_unref(b->io_event_source);
        sd_event_source_unref(b->check_event_source);

        if (b->fd >= 0)
                /* If the device has been unplugged close() returns
                 * ENODEV, let's ignore this, hence we don't use
                 * safe_close() */
                (void) close(b->fd);

        free(b->name);
        free(b->seat);
        free(b);
}