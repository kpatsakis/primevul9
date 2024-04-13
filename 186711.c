static errno_t sdap_ad_resolve_sids_step(struct tevent_req *req)
{
    struct sdap_ad_resolve_sids_state *state = NULL;
    struct tevent_req *subreq = NULL;
    struct sdap_domain *sdap_domain = NULL;
    struct sss_domain_info *domain = NULL;

    state = tevent_req_data(req, struct sdap_ad_resolve_sids_state);

    do {
        state->current_sid = state->sids[state->index];
        if (state->current_sid == NULL) {
            return EOK;
        }
        state->index++;

        domain = sss_get_domain_by_sid_ldap_fallback(state->domain,
                                                     state->current_sid);

        if (domain == NULL) {
            DEBUG(SSSDBG_MINOR_FAILURE, "SID %s does not belong to any known "
                                         "domain\n", state->current_sid);
        }
    } while (domain == NULL);

    sdap_domain = sdap_domain_get(state->opts, domain);
    if (sdap_domain == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "SDAP domain does not exist?\n");
        return ERR_INTERNAL;
    }

    subreq = groups_get_send(state, state->ev, state->id_ctx, sdap_domain,
                             state->conn, state->current_sid,
                             BE_FILTER_SECID, BE_ATTR_CORE, false);
    if (subreq == NULL) {
        return ENOMEM;
    }

    tevent_req_set_callback(subreq, sdap_ad_resolve_sids_done, req);

    return EAGAIN;
}