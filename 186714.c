sdap_ad_resolve_sids_send(TALLOC_CTX *mem_ctx,
                          struct tevent_context *ev,
                          struct sdap_id_ctx *id_ctx,
                          struct sdap_id_conn_ctx *conn,
                          struct sdap_options *opts,
                          struct sss_domain_info *domain,
                          char **sids)
{
    struct sdap_ad_resolve_sids_state *state = NULL;
    struct tevent_req *req = NULL;
    errno_t ret;

    req = tevent_req_create(mem_ctx, &state,
                            struct sdap_ad_resolve_sids_state);
    if (req == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "tevent_req_create() failed\n");
        return NULL;
    }

    state->ev = ev;
    state->id_ctx = id_ctx;
    state->conn = conn;
    state->opts = opts;
    state->domain = get_domains_head(domain);
    state->sids = sids;
    state->index = 0;

    if (state->sids == NULL || state->sids[0] == NULL) {
        ret = EOK;
        goto immediately;
    }

    ret = sdap_ad_resolve_sids_step(req);
    if (ret != EAGAIN) {
        goto immediately;
    }

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