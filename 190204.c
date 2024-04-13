AP_DECLARE(int) ap_find_child_by_pid(apr_proc_t *pid)
{
    int i;
    int max_daemons_limit = 0;

    ap_mpm_query(AP_MPMQ_MAX_DAEMON_USED, &max_daemons_limit);

    for (i = 0; i < max_daemons_limit; ++i) {
        if (ap_scoreboard_image->parent[i].pid == pid->pid) {
            return i;
        }
    }

    return -1;
}