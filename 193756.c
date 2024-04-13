crm_ipcs_client_pid(qb_ipcs_connection_t * c)
{
    struct qb_ipcs_connection_stats stats;

    stats.client_pid = 0;
    qb_ipcs_connection_stats_get(c, &stats, 0);
    return stats.client_pid;
}