static bool manager_get_show_status(Manager *m, StatusType type) {
        assert(m);

        if (m->running_as != SYSTEMD_SYSTEM)
                return false;

        if (m->no_console_output)
                return false;

        if (!IN_SET(manager_state(m), MANAGER_INITIALIZING, MANAGER_STARTING, MANAGER_STOPPING))
                return false;

        /* If we cannot find out the status properly, just proceed. */
        if (type != STATUS_TYPE_EMERGENCY && manager_check_ask_password(m) > 0)
                return false;

        if (m->show_status > 0)
                return true;

        /* If Plymouth is running make sure we show the status, so
         * that there's something nice to see when people press Esc */
        return plymouth_running();
}