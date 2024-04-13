static int button_recheck(sd_event_source *e, void *userdata) {
        Button *b = userdata;

        assert(b);
        assert(b->lid_closed);

        button_lid_switch_handle_action(b->manager, false);
        return 1;
}