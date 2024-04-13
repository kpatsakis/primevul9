static void wsgi_manage_process(int reason, void *data, apr_wait_t status)
{
    WSGIDaemonProcess *daemon = data;

    switch (reason) {

        /* Child daemon process has died. */

        case APR_OC_REASON_DEATH: {
            int mpm_state;
            int stopping;

            /* Stop watching the existing process. */

            apr_proc_other_child_unregister(daemon);

            /*
             * Determine if Apache is being shutdown or not and
             * if it is not being shutdown, restart the child
             * daemon process that has died. If MPM doesn't
             * support query assume that child daemon process
             * shouldn't be restarted. Both prefork and worker
             * MPMs support this query so should always be okay.
             */

            stopping = 1;

            if (ap_mpm_query(AP_MPMQ_MPM_STATE, &mpm_state) == APR_SUCCESS
                && mpm_state != AP_MPMQ_STOPPING) {
                stopping = 0;
            }

            if (!stopping) {
                ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0),
                             wsgi_server, "mod_wsgi (pid=%d): "
                             "Process '%s' has died, restarting.",
                             daemon->process.pid, daemon->group->name);

                if (WIFEXITED(status)) {
                    ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0),
                             wsgi_server, "mod_wsgi (pid=%d): "
                             "Process '%s' terminated normally, exit code %d", 
                             daemon->process.pid, daemon->group->name,
                             WEXITSTATUS(status));
                }
                else if (WIFSIGNALED(status)) {
                    ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0),
                             wsgi_server, "mod_wsgi (pid=%d): "
                             "Process '%s' terminated by signal %d", 
                             daemon->process.pid, daemon->group->name,
                             WTERMSIG(status));
                }

                wsgi_start_process(wsgi_parent_pool, daemon);
            }

            break;
        }

        /* Apache is being restarted or shutdown. */

        case APR_OC_REASON_RESTART: {

            /* Stop watching the existing process. */

            apr_proc_other_child_unregister(daemon);

            break;
        }

        /* Child daemon process vanished. */

        case APR_OC_REASON_LOST: {

            /* Stop watching the existing process. */

            apr_proc_other_child_unregister(daemon);

            /* Restart the child daemon process that has died. */

            ap_log_error(APLOG_MARK, WSGI_LOG_INFO(0),
                         wsgi_server, "mod_wsgi (pid=%d): "
                         "Process '%s' has died, restarting.",
                         daemon->process.pid, daemon->group->name);

            wsgi_start_process(wsgi_parent_pool, daemon);

            break;
        }

        /* Call to unregister the process. */

        case APR_OC_REASON_UNREGISTER: {

            /* Nothing to do at present. */

            break;
        }
    }
}