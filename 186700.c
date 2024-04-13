static void sdap_ad_tokengroups_initgr_mapping_done(struct tevent_req *subreq)
{
    TALLOC_CTX *tmp_ctx = NULL;
    struct sdap_ad_tokengroups_initgr_mapping_state *state = NULL;
    struct tevent_req *req = NULL;
    struct sss_domain_info *domain = NULL;
    struct ldb_message *msg = NULL;
    const char *attrs[] = {SYSDB_NAME, NULL};
    const char *name = NULL;
    const char *sid = NULL;
    char **sids = NULL;
    size_t num_sids = 0;
    size_t i;
    time_t now;
    gid_t gid;
    char **groups = NULL;
    size_t num_groups;
    errno_t ret, sret;
    bool in_transaction = false;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "talloc_new() failed\n");
        ret = ENOMEM;
        goto done;
    }

    req = tevent_req_callback_data(subreq, struct tevent_req);
    state = tevent_req_data(req, struct sdap_ad_tokengroups_initgr_mapping_state);

    ret = sdap_get_ad_tokengroups_recv(state, subreq, &num_sids, &sids);
    talloc_zfree(subreq);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Unable to acquire tokengroups [%d]: %s\n",
                                    ret, strerror(ret));
        goto done;
    }

    num_groups = 0;
    groups = talloc_zero_array(tmp_ctx, char*, num_sids + 1);
    if (groups == NULL) {
        ret = ENOMEM;
        goto done;
    }

    now = time(NULL);
    ret = sysdb_transaction_start(state->sysdb);
    if (ret != EOK) {
        goto done;
    }
    in_transaction = true;

    for (i = 0; i < num_sids; i++) {
        sid = sids[i];
        DEBUG(SSSDBG_TRACE_LIBS, "Processing membership SID [%s]\n", sid);

        ret = sdap_idmap_sid_to_unix(state->idmap_ctx, sid, &gid);
        if (ret == ENOTSUP) {
            DEBUG(SSSDBG_TRACE_FUNC, "Skipping built-in object.\n");
            ret = EOK;
            continue;
        } else if (ret != EOK) {
            DEBUG(SSSDBG_MINOR_FAILURE, "Could not convert SID to GID: [%s]. "
                                         "Skipping\n", strerror(ret));
            continue;
        }

        domain = sss_get_domain_by_sid_ldap_fallback(state->domain, sid);
        if (domain == NULL) {
            DEBUG(SSSDBG_MINOR_FAILURE, "Domain not found for SID %s\n", sid);
            continue;
        }

        DEBUG(SSSDBG_TRACE_LIBS, "SID [%s] maps to GID [%"SPRIgid"]\n",
                                  sid, gid);

        /* Check whether this GID already exists in the sysdb */
        ret = sysdb_search_group_by_gid(tmp_ctx, domain->sysdb, domain,
                                        gid, attrs, &msg);
        if (ret == EOK) {
            name = ldb_msg_find_attr_as_string(msg, SYSDB_NAME, NULL);
            if (name == NULL) {
                DEBUG(SSSDBG_MINOR_FAILURE,
                      "Could not retrieve group name from sysdb\n");
                ret = EINVAL;
                goto done;
            }
        } else if (ret == ENOENT) {
            /* This is a new group. For now, we will store it under the name
             * of its SID. When a direct lookup of the group or its GID occurs,
             * it will replace this temporary entry. */
            name = sid;
            ret = sysdb_add_incomplete_group(domain->sysdb, domain, name, gid,
                                             NULL, sid, false, now);
            if (ret != EOK) {
                DEBUG(SSSDBG_MINOR_FAILURE, "Could not create incomplete "
                                             "group: [%s]\n", strerror(ret));
                goto done;
            }
        } else {
            /* Unexpected error */
            DEBUG(SSSDBG_MINOR_FAILURE, "Could not look up group in sysdb: "
                                         "[%s]\n", strerror(ret));
            goto done;
        }

        groups[num_groups] = sysdb_group_strdn(tmp_ctx, domain->name, name);
        if (groups[num_groups] == NULL) {
            ret = ENOMEM;
            goto done;
        }
        num_groups++;
    }

    groups[num_groups] = NULL;

    ret = sdap_ad_tokengroups_update_members(state, state->username,
                                             state->sysdb, state->domain,
                                             groups);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Membership update failed [%d]: %s\n",
                                     ret, strerror(ret));
        goto done;
    }

    ret = sysdb_transaction_commit(state->sysdb);
    if (ret != EOK) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Could not commit transaction! [%s]\n",
                                    strerror(ret));
        goto done;
    }
    in_transaction = false;

done:
    talloc_free(tmp_ctx);

    if (in_transaction) {
        sret = sysdb_transaction_cancel(state->sysdb);
        DEBUG(SSSDBG_FATAL_FAILURE, "Could not cancel transaction! [%s]\n",
                                     strerror(sret));
    }

    if (ret != EOK) {
        tevent_req_error(req, ret);
        return;
    }

    tevent_req_done(req);
}