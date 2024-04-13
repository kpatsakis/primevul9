sdap_get_ad_tokengroups_send(TALLOC_CTX *mem_ctx,
                             struct tevent_context *ev,
                             struct sdap_options *opts,
                             struct sdap_handle *sh,
                             const char *name,
                             const char *orig_dn,
                             int timeout)
{
    struct sdap_get_ad_tokengroups_state *state = NULL;
    struct tevent_req *req = NULL;
    struct tevent_req *subreq = NULL;
    const char *attrs[] = {AD_TOKENGROUPS_ATTR, NULL};
    errno_t ret;

    req = tevent_req_create(mem_ctx, &state,
                            struct sdap_get_ad_tokengroups_state);
    if (req == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "tevent_req_create() failed\n");
        return NULL;
    }

    state->idmap_ctx = opts->idmap_ctx->map;
    state->ev = ev;
    state->username = talloc_strdup(state, name);
    if (state->username == NULL) {
        ret = ENOMEM;
        goto immediately;
    }

    subreq = sdap_get_generic_send(state, state->ev, opts, sh, orig_dn,
                                   LDAP_SCOPE_BASE, NULL, attrs,
                                   NULL, 0, timeout, false);
    if (subreq == NULL) {
        ret = ENOMEM;
        goto immediately;
    }

    tevent_req_set_callback(subreq, sdap_get_ad_tokengroups_done, req);

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