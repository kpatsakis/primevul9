void manager_send_unit_audit(Manager *m, Unit *u, int type, bool success) {

#ifdef HAVE_AUDIT
        _cleanup_free_ char *p = NULL;
        const char *msg;
        int audit_fd;

        audit_fd = get_audit_fd();
        if (audit_fd < 0)
                return;

        /* Don't generate audit events if the service was already
         * started and we're just deserializing */
        if (m->n_reloading > 0)
                return;

        if (m->running_as != SYSTEMD_SYSTEM)
                return;

        if (u->type != UNIT_SERVICE)
                return;

        p = unit_name_to_prefix_and_instance(u->id);
        if (!p) {
                log_oom();
                return;
        }

        msg = strappenda("unit=", p);

        if (audit_log_user_comm_message(audit_fd, type, msg, "systemd", NULL, NULL, NULL, success) < 0) {
                if (errno == EPERM)
                        /* We aren't allowed to send audit messages?
                         * Then let's not retry again. */
                        close_audit_fd();
                else
                        log_warning_errno(errno, "Failed to send audit message: %m");
        }
#endif

}