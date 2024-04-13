static int button_suitable(Button *b) {
        unsigned long types[CONST_MAX(EV_KEY, EV_SW)/ULONG_BITS+1];

        assert(b);
        assert(b->fd);

        if (ioctl(b->fd, EVIOCGBIT(EV_SYN, sizeof(types)), types) < 0)
                return -errno;

        if (bitset_get(types, EV_KEY)) {
                unsigned long keys[CONST_MAX4(KEY_POWER, KEY_POWER2, KEY_SLEEP, KEY_SUSPEND)/ULONG_BITS+1];

                if (ioctl(b->fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys) < 0)
                        return -errno;

                if (bitset_get(keys, KEY_POWER) ||
                    bitset_get(keys, KEY_POWER2) ||
                    bitset_get(keys, KEY_SLEEP) ||
                    bitset_get(keys, KEY_SUSPEND))
                        return true;
        }

        if (bitset_get(types, EV_SW)) {
                unsigned long switches[CONST_MAX(SW_LID, SW_DOCK)/ULONG_BITS+1];

                if (ioctl(b->fd, EVIOCGBIT(EV_SW, sizeof(switches)), switches) < 0)
                        return -errno;

                if (bitset_get(switches, SW_LID) ||
                    bitset_get(switches, SW_DOCK))
                        return true;
        }

        return false;
}