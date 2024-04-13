int button_check_switches(Button *b) {
        unsigned long switches[CONST_MAX(SW_LID, SW_DOCK)/ULONG_BITS+1] = {};
        assert(b);

        if (b->fd < 0)
                return -EINVAL;

        if (ioctl(b->fd, EVIOCGSW(sizeof(switches)), switches) < 0)
                return -errno;

        b->lid_closed = bitset_get(switches, SW_LID);
        b->docked = bitset_get(switches, SW_DOCK);

        if (b->lid_closed)
                button_install_check_event_source(b);

        return 0;
}