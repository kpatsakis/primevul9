sdap_get_ad_match_rule_initgroups_next_base(struct tevent_req *req)
{
    struct tevent_req *subreq;
    struct sdap_ad_match_rule_initgr_state *state;

    state = tevent_req_data(req, struct sdap_ad_match_rule_initgr_state);

    talloc_zfree(state->filter);
    state->filter = sdap_get_id_specific_filter(state,
                        state->base_filter,
                        state->search_bases[state->base_iter]->filter);
    if (!state->filter) {
        return ENOMEM;
    }

    DEBUG(SSSDBG_TRACE_FUNC,
          "Searching for groups with base [%s]\n",
           state->search_bases[state->base_iter]->basedn);

    subreq = sdap_get_generic_send(
            state, state->ev, state->opts, state->sh,
            state->search_bases[state->base_iter]->basedn,
            state->search_bases[state->base_iter]->scope,
            state->filter, state->attrs,
            state->opts->group_map, SDAP_OPTS_GROUP,
            state->timeout, true);
    if (!subreq) {
        return ENOMEM;
    }

    tevent_req_set_callback(subreq,
                            sdap_get_ad_match_rule_initgroups_step,
                            req);

    return EOK;
}