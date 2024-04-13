static int button_install_check_event_source(Button *b) {
        int r;
        assert(b);

        /* Install a post handler, so that we keep rechecking as long as the lid is closed. */

        if (b->check_event_source)
                return 0;

        r = sd_event_add_post(b->manager->event, &b->check_event_source, button_recheck, b);
        if (r < 0)
                return r;

        return sd_event_source_set_priority(b->check_event_source, SD_EVENT_PRIORITY_IDLE+1);
}