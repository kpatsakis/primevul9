static void krb5_find_ccache_step(struct tevent_req *req)
{
    struct krb5_auth_state *state = tevent_req_data(req, struct krb5_auth_state);
    int ret;
    struct krb5child_req *kr = state->kr;
    struct pam_data *pd = kr->pd;
    char *msg;
    bool private_path = false;
    struct tevent_req *subreq = NULL;

    if (!kr->is_offline) {
        kr->is_offline = be_is_offline(state->be_ctx);
    }

    /* The ccache file should be (re)created if one of the following conditions
     * is true:
     * - it doesn't exist (kr->ccname == NULL)
     * - the backend is online and the current ccache file is not used, i.e
     *   the related user is currently not logged in and it is not a renewal
     *   request
     *   (!kr->is_offline && !kr->active_ccache_present &&
     *    pd->cmd != SSS_CMD_RENEW)
     * - the backend is offline and the current cache file not used and
     *   it does not contain a valid tgt
     *   (kr->is_offline &&
     *    !kr->active_ccache_present && !kr->valid_tgt_present)
     */
    if (kr->ccname == NULL ||
        (kr->is_offline && !kr->active_ccache_present &&
            !kr->valid_tgt_present) ||
        (!kr->is_offline && !kr->active_ccache_present &&
         pd->cmd != SSS_CMD_RENEW)) {
            DEBUG(9, ("Recreating  ccache file.\n"));
            kr->ccname = expand_ccname_template(kr, kr,
                                          dp_opt_get_cstring(kr->krb5_ctx->opts,
                                                             KRB5_CCNAME_TMPL),
                                                true, &private_path);
            if (kr->ccname == NULL) {
                DEBUG(1, ("expand_ccname_template failed.\n"));
                ret = ENOMEM;
                goto done;
            }

            ret = create_ccache_dir(kr, kr->ccname,
                                    kr->krb5_ctx->illegal_path_re,
                                    kr->uid, kr->gid, private_path);
            if (ret != EOK) {
                DEBUG(1, ("create_ccache_dir failed.\n"));
                goto done;
            }
    }

    if (kr->is_offline) {
        DEBUG(9, ("Preparing for offline operation.\n"));

        if (kr->valid_tgt_present || kr->active_ccache_present) {
            DEBUG(9, ("Valid TGT available or "
                      "ccache file is already in use.\n"));
            kr->ccname = kr->old_ccname;
            msg = talloc_asprintf(pd, "%s=%s", CCACHE_ENV_NAME, kr->ccname);
            if (msg == NULL) {
                DEBUG(1, ("talloc_asprintf failed.\n"));
            } else {
                ret = pam_add_response(pd, SSS_PAM_ENV_ITEM, strlen(msg) + 1,
                                       (uint8_t *) msg);
                if (ret != EOK) {
                    DEBUG(1, ("pam_add_response failed.\n"));
                }
            }

            if (dp_opt_get_bool(kr->krb5_ctx->opts,
                                KRB5_STORE_PASSWORD_IF_OFFLINE)) {
                krb5_pam_handler_cache_auth_step(req);
                return;
            }

            state->pam_status = PAM_AUTHINFO_UNAVAIL;
            state->dp_err = DP_ERR_OFFLINE;
            ret = EOK;
            goto done;

        }
    }

    /* We need to keep the root privileges to read the keytab file if
     * validation or FAST is enabled, otherwise we can drop them and run
     * krb5_child with user privileges.
     * If we are offline we want to create an empty ccache file. In this
     * case we can drop the privileges, too. */
    if ((dp_opt_get_bool(kr->krb5_ctx->opts, KRB5_VALIDATE) ||
         kr->krb5_ctx->use_fast) &&
        !kr->is_offline) {
        kr->run_as_user = false;
    } else {
        kr->run_as_user = true;
    }

    subreq = handle_child_send(state, state->ev, kr);
    if (subreq == NULL) {
        DEBUG(1, ("handle_child_send failed.\n"));
        ret = ENOMEM;
        goto done;
    }

    tevent_req_set_callback(subreq, krb5_child_done, req);
    return;

done:
    if (ret == EOK) {
        tevent_req_done(req);
    } else {
        tevent_req_error(req, ret);
    }
}