static int button_set_mask(Button *b) {
        unsigned long
                types[CONST_MAX(EV_KEY, EV_SW)/ULONG_BITS+1] = {},
                keys[CONST_MAX4(KEY_POWER, KEY_POWER2, KEY_SLEEP, KEY_SUSPEND)/ULONG_BITS+1] = {},
                switches[CONST_MAX(SW_LID, SW_DOCK)/ULONG_BITS+1] = {};
        struct input_mask mask;

        assert(b);
        assert(b->fd >= 0);

        bitset_put(types, EV_KEY);
        bitset_put(types, EV_SW);

        mask = (struct input_mask) {
                .type = EV_SYN,
                .codes_size = sizeof(types),
                .codes_ptr = PTR_TO_UINT64(types),
        };

        if (ioctl(b->fd, EVIOCSMASK, &mask) < 0)
                /* Log only at debug level if the kernel doesn't do EVIOCSMASK yet */
                return log_full_errno(IN_SET(errno, ENOTTY, EOPNOTSUPP, EINVAL) ? LOG_DEBUG : LOG_WARNING,
                                      errno, "Failed to set EV_SYN event mask on /dev/input/%s: %m", b->name);

        bitset_put(keys, KEY_POWER);
        bitset_put(keys, KEY_POWER2);
        bitset_put(keys, KEY_SLEEP);
        bitset_put(keys, KEY_SUSPEND);

        mask = (struct input_mask) {
                .type = EV_KEY,
                .codes_size = sizeof(keys),
                .codes_ptr = PTR_TO_UINT64(keys),
        };

        if (ioctl(b->fd, EVIOCSMASK, &mask) < 0)
                return log_warning_errno(errno, "Failed to set EV_KEY event mask on /dev/input/%s: %m", b->name);

        bitset_put(switches, SW_LID);
        bitset_put(switches, SW_DOCK);

        mask = (struct input_mask) {
                .type = EV_SW,
                .codes_size = sizeof(switches),
                .codes_ptr = PTR_TO_UINT64(switches),
        };

        if (ioctl(b->fd, EVIOCSMASK, &mask) < 0)
                return log_warning_errno(errno, "Failed to set EV_SW event mask on /dev/input/%s: %m", b->name);

        return 0;
}