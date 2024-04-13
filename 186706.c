sdap_ad_tokengroups_initgroups_send(TALLOC_CTX *mem_ctx,
                                    struct tevent_context *ev,
                                    struct sdap_id_ctx *id_ctx,
                                    struct sdap_id_conn_ctx *conn,
                                    struct sdap_options *opts,
                                    struct sysdb_ctx *sysdb,
                                    struct sss_domain_info *domain,
                                    struct sdap_handle *sh,
                                    const char *name,
                                    const char *orig_dn,
                                    int timeout,
                                    bool use_id_mapping)
{
    struct sdap_ad_tokengroups_initgroups_state *state = NULL;
    struct tevent_req *req = NULL;
    struct tevent_req *subreq = NULL;
    errno_t ret;

    req = tevent_req_create(mem_ctx, &state,
                            struct sdap_ad_tokengroups_initgroups_state);
    if (req == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "tevent_req_create() failed\n");
        return NULL;
    }

    state->use_id_mapping = use_id_mapping;
    state->domain = domain;

    if (state->use_id_mapping && !IS_SUBDOMAIN(state->domain)) {
        subreq = sdap_ad_tokengroups_initgr_mapping_send(state, ev, opts,
                                                         sysdb, domain, sh,
                                                         name, orig_dn,
                                                         timeout);
    } else {
        subreq = sdap_ad_tokengroups_initgr_posix_send(state, ev, id_ctx, conn,
                                                       opts, sysdb, domain, sh,
                                                       name, orig_dn,
                                                       timeout);
    }
    if (subreq == NULL) {
        ret = ENOMEM;
        goto immediately;
    }

    tevent_req_set_callback(subreq, sdap_ad_tokengroups_initgroups_done, req);

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