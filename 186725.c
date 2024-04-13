sdap_ad_tokengroups_initgr_posix_send(TALLOC_CTX *mem_ctx,
                                      struct tevent_context *ev,
                                      struct sdap_id_ctx *id_ctx,
                                      struct sdap_id_conn_ctx *conn,
                                      struct sdap_options *opts,
                                      struct sysdb_ctx *sysdb,
                                      struct sss_domain_info *domain,
                                      struct sdap_handle *sh,
                                      const char *name,
                                      const char *orig_dn,
                                      int timeout)
{
    struct sdap_ad_tokengroups_initgr_posix_state *state = NULL;
    struct tevent_req *req = NULL;
    struct tevent_req *subreq = NULL;
    struct sdap_domain *sdom;
    struct ad_id_ctx *subdom_id_ctx;
    errno_t ret;

    req = tevent_req_create(mem_ctx, &state,
                            struct sdap_ad_tokengroups_initgr_posix_state);
    if (req == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "tevent_req_create() failed\n");
        return NULL;
    }

    state->ev = ev;
    state->id_ctx = id_ctx;
    state->conn = conn;
    state->opts = opts;
    state->sh = sh;
    state->sysdb = sysdb;
    state->domain = domain;
    state->orig_dn = orig_dn;
    state->timeout = timeout;
    state->username = talloc_strdup(state, name);
    if (state->username == NULL) {
        ret = ENOMEM;
        goto immediately;
    }

    sdom = sdap_domain_get(opts, domain);
    if (sdom == NULL || sdom->pvt == NULL) {
        ret = handle_missing_pvt(mem_ctx, ev, opts, orig_dn, timeout,
                                 state->username, sh, req,
                                 sdap_ad_tokengroups_initgr_posix_tg_done);
        if (ret == EOK) {
            return req;
        } else {
            DEBUG(SSSDBG_CRIT_FAILURE, "No ID ctx available for [%s].\n",
                  domain->name);
            goto immediately;
        }
    }
    subdom_id_ctx = talloc_get_type(sdom->pvt, struct ad_id_ctx);
    state->op = sdap_id_op_create(state, subdom_id_ctx->ldap_ctx->conn_cache);
    if (!state->op) {
        DEBUG(SSSDBG_OP_FAILURE, "sdap_id_op_create failed\n");
        ret = ENOMEM;
        goto immediately;
    }

    subreq = sdap_id_op_connect_send(state->op, state, &ret);
    if (subreq == NULL) {
        ret = ENOMEM;
        goto immediately;
    }

    tevent_req_set_callback(subreq,
                            sdap_ad_tokengroups_initgr_posix_sids_connect_done,
                            req);

    return req;

immediately:
    if (ret == EOK) {
        tevent_req_done(req);
    } else {
        tevent_req_error(req, ret);
    }
    tevent_req_post(req, ev);

    return req;
}