static void sdap_get_ad_tokengroups_done(struct tevent_req *subreq)
{
    TALLOC_CTX *tmp_ctx = NULL;
    struct sdap_get_ad_tokengroups_state *state = NULL;
    struct tevent_req *req = NULL;
    struct sysdb_attrs **users = NULL;
    struct ldb_message_element *el = NULL;
    enum idmap_error_code err;
    char *sid_str = NULL;
    size_t num_users;
    size_t i;
    errno_t ret;

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_get_ad_tokengroups_state);

    ret = sdap_get_generic_recv(subreq, tmp_ctx, &num_users, &users);
    talloc_zfree(subreq);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "LDAP search failed: [%s]\n", sss_strerror(ret));
        goto done;
    }

    if (num_users != 1) {
        DEBUG(SSSDBG_MINOR_FAILURE,
              "More than one result on a base search!\n");
        ret = EINVAL;
        goto done;
    }

    /* get the list of sids from tokengroups */
    ret = sysdb_attrs_get_el_ext(users[0], AD_TOKENGROUPS_ATTR, false, &el);
    if (ret == ENOENT) {
        DEBUG(SSSDBG_TRACE_LIBS, "No tokenGroups entries for [%s]\n",
                                  state->username);

        state->sids = NULL;
        state->num_sids = 0;
        ret = EOK;
        goto done;
    } else if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Could not read tokenGroups attribute: "
                                     "[%s]\n", strerror(ret));
        goto done;
    }

    state->num_sids = 0;
    state->sids = talloc_zero_array(state, char*, el->num_values);
    if (state->sids == NULL) {
        ret = ENOMEM;
        goto done;
    }

    /* convert binary sid to string */
    for (i = 0; i < el->num_values; i++) {
        err = sss_idmap_bin_sid_to_sid(state->idmap_ctx, el->values[i].data,
                                       el->values[i].length, &sid_str);
        if (err != IDMAP_SUCCESS) {
            DEBUG(SSSDBG_MINOR_FAILURE,
                  "Could not convert binary SID to string: [%s]. Skipping\n",
                   idmap_error_string(err));
            continue;
        }

        state->sids[i] = talloc_move(state->sids, &sid_str);
        state->num_sids++;
    }

    /* shrink array to final number of elements */
    state->sids = talloc_realloc(state, state->sids, char*, state->num_sids);
    if (state->sids == NULL) {
        ret = ENOMEM;
        goto done;
    }

    ret = EOK;

done:
    talloc_free(tmp_ctx);

    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    tevent_req_done(req);
}