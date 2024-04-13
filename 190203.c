static int update_child_status_internal(int child_num,
                                        int thread_num,
                                        int status,
                                        conn_rec *c,
                                        server_rec *s,
                                        request_rec *r,
                                        const char *descr)
{
    int old_status;
    worker_score *ws;
    int mpm_generation;

    ws = &ap_scoreboard_image->servers[child_num][thread_num];
    old_status = ws->status;
    ws->status = status;
    
    if (status == SERVER_READY
        && old_status == SERVER_STARTING) {
        process_score *ps = &ap_scoreboard_image->parent[child_num];
        ws->thread_num = child_num * thread_limit + thread_num;
        ap_mpm_query(AP_MPMQ_GENERATION, &mpm_generation);
        ps->generation = mpm_generation;
    }

    if (ap_extended_status) {
        const char *val;
        
        if (status == SERVER_READY || status == SERVER_DEAD) {
            /*
             * Reset individual counters
             */
            if (status == SERVER_DEAD) {
                ws->my_access_count = 0L;
                ws->my_bytes_served = 0L;
#ifdef HAVE_TIMES
                ws->times.tms_utime = 0;
                ws->times.tms_stime = 0;
                ws->times.tms_cutime = 0;
                ws->times.tms_cstime = 0;
#endif
            }
            ws->conn_count = 0;
            ws->conn_bytes = 0;
            ws->last_used = apr_time_now();
        }

        if (descr) {
            apr_cpystrn(ws->request, descr, sizeof(ws->request));
        }
        else if (r) {
            copy_request(ws->request, sizeof(ws->request), r);
        }
        else if (c) {
            ws->request[0]='\0';
        }

        if (r && r->useragent_ip) {
            if (!(val = ap_get_useragent_host(r, REMOTE_NOLOOKUP, NULL))) {
                apr_cpystrn(ws->client, r->useragent_ip, sizeof(ws->client)); /* DEPRECATE */
                apr_cpystrn(ws->client64, r->useragent_ip, sizeof(ws->client64));
            }
            else {
                apr_cpystrn(ws->client, val, sizeof(ws->client)); /* DEPRECATE */
                apr_cpystrn(ws->client64, val, sizeof(ws->client64));
            }
        }
        else if (c) {
            if (!(val = ap_get_remote_host(c, c->base_server->lookup_defaults,
                                           REMOTE_NOLOOKUP, NULL))) {
                apr_cpystrn(ws->client, c->client_ip, sizeof(ws->client)); /* DEPRECATE */
                apr_cpystrn(ws->client64, c->client_ip, sizeof(ws->client64));
            }
            else {
                apr_cpystrn(ws->client, val, sizeof(ws->client)); /* DEPRECATE */
                apr_cpystrn(ws->client64, val, sizeof(ws->client64));
            }
        }

        if (s) {
            if (c) {
                apr_snprintf(ws->vhost, sizeof(ws->vhost), "%s:%d",
                             s->server_hostname, c->local_addr->port);
            }
            else {
                apr_cpystrn(ws->vhost, s->server_hostname, sizeof(ws->vhost));
            }
        }
        else if (c) {
            ws->vhost[0]='\0';
        }

        if (c) {
            val = ap_get_protocol(c);
            apr_cpystrn(ws->protocol, val, sizeof(ws->protocol));
        }
    }

    return old_status;
}