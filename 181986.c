static int button_dispatch(sd_event_source *s, int fd, uint32_t revents, void *userdata) {
        Button *b = userdata;
        struct input_event ev;
        ssize_t l;

        assert(s);
        assert(fd == b->fd);
        assert(b);

        l = read(b->fd, &ev, sizeof(ev));
        if (l < 0)
                return errno != EAGAIN ? -errno : 0;
        if ((size_t) l < sizeof(ev))
                return -EIO;

        if (ev.type == EV_KEY && ev.value > 0) {

                switch (ev.code) {

                case KEY_POWER:
                case KEY_POWER2:
                        log_struct(LOG_INFO,
                                   LOG_MESSAGE("Power key pressed."),
                                   "MESSAGE_ID=" SD_MESSAGE_POWER_KEY_STR);

                        manager_handle_action(b->manager, INHIBIT_HANDLE_POWER_KEY, b->manager->handle_power_key, b->manager->power_key_ignore_inhibited, true);
                        break;

                /* The kernel is a bit confused here:

                   KEY_SLEEP   = suspend-to-ram, which everybody else calls "suspend"
                   KEY_SUSPEND = suspend-to-disk, which everybody else calls "hibernate"
                */

                case KEY_SLEEP:
                        log_struct(LOG_INFO,
                                   LOG_MESSAGE("Suspend key pressed."),
                                   "MESSAGE_ID=" SD_MESSAGE_SUSPEND_KEY_STR);

                        manager_handle_action(b->manager, INHIBIT_HANDLE_SUSPEND_KEY, b->manager->handle_suspend_key, b->manager->suspend_key_ignore_inhibited, true);
                        break;

                case KEY_SUSPEND:
                        log_struct(LOG_INFO,
                                   LOG_MESSAGE("Hibernate key pressed."),
                                   "MESSAGE_ID=" SD_MESSAGE_HIBERNATE_KEY_STR);

                        manager_handle_action(b->manager, INHIBIT_HANDLE_HIBERNATE_KEY, b->manager->handle_hibernate_key, b->manager->hibernate_key_ignore_inhibited, true);
                        break;
                }

        } else if (ev.type == EV_SW && ev.value > 0) {

                if (ev.code == SW_LID) {
                        log_struct(LOG_INFO,
                                   LOG_MESSAGE("Lid closed."),
                                   "MESSAGE_ID=" SD_MESSAGE_LID_CLOSED_STR);

                        b->lid_closed = true;
                        button_lid_switch_handle_action(b->manager, true);
                        button_install_check_event_source(b);

                } else if (ev.code == SW_DOCK) {
                        log_struct(LOG_INFO,
                                   LOG_MESSAGE("System docked."),
                                   "MESSAGE_ID=" SD_MESSAGE_SYSTEM_DOCKED_STR);

                        b->docked = true;
                }

        } else if (ev.type == EV_SW && ev.value == 0) {

                if (ev.code == SW_LID) {
                        log_struct(LOG_INFO,
                                   LOG_MESSAGE("Lid opened."),
                                   "MESSAGE_ID=" SD_MESSAGE_LID_OPENED_STR);

                        b->lid_closed = false;
                        b->check_event_source = sd_event_source_unref(b->check_event_source);

                } else if (ev.code == SW_DOCK) {
                        log_struct(LOG_INFO,
                                   LOG_MESSAGE("System undocked."),
                                   "MESSAGE_ID=" SD_MESSAGE_SYSTEM_UNDOCKED_STR);

                        b->docked = false;
                }
        }

        return 0;
}