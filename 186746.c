struct tevent_req *krb5_auth_send(TALLOC_CTX *mem_ctx,
                                  struct tevent_context *ev,
                                  struct be_ctx *be_ctx,
                                  struct pam_data *pd,
                                  struct krb5_ctx *krb5_ctx)
{
    const char **attrs;
    struct krb5_auth_state *state;
    struct ldb_result *res;
    struct krb5child_req *kr = NULL;
    const char *ccache_file = NULL;
    const char *realm;
    krb5_error_code kerr;
    struct tevent_req *req;
    struct tevent_req *subreq;
    int ret;

    req = tevent_req_create(mem_ctx, &state, struct krb5_auth_state);
    if (req == NULL) {
        DEBUG(1, ("tevent_req_create failed.\n"));
        return NULL;
    }

    state->ev = ev;
    state->be_ctx = be_ctx;
    state->pd = pd;
    state->krb5_ctx = krb5_ctx;
    state->kr = NULL;
    state->pam_status = PAM_SYSTEM_ERR;
    state->dp_err = DP_ERR_FATAL;

    switch (pd->cmd) {
        case SSS_PAM_AUTHENTICATE:
        case SSS_CMD_RENEW:
        case SSS_PAM_CHAUTHTOK:
            break;
        case SSS_PAM_CHAUTHTOK_PRELIM:
            if (pd->priv == 1 && pd->authtok_size == 0) {
                DEBUG(4, ("Password reset by root is not supported.\n"));
                state->pam_status = PAM_PERM_DENIED;
                state->dp_err = DP_ERR_OK;
                ret = EOK;
                goto done;
            }
            break;
        default:
            DEBUG(4, ("Unexpected pam task %d.\n", pd->cmd));
            state->pam_status = PAM_SYSTEM_ERR;
            state->dp_err = DP_ERR_FATAL;
            ret = EINVAL;
            goto done;
    }

    if (be_is_offline(be_ctx) &&
        (pd->cmd == SSS_PAM_CHAUTHTOK || pd->cmd == SSS_PAM_CHAUTHTOK_PRELIM ||
         pd->cmd == SSS_CMD_RENEW)) {
        DEBUG(9, ("Password changes and ticket renewal are not possible "
                  "while offline.\n"));
        state->pam_status = PAM_AUTHINFO_UNAVAIL;
        state->dp_err = DP_ERR_OFFLINE;
        ret = EOK;
        goto done;
    }

    attrs = talloc_array(state, const char *, 6);
    if (attrs == NULL) {
        ret = ENOMEM;
        goto done;
    }

    attrs[0] = SYSDB_UPN;
    attrs[1] = SYSDB_HOMEDIR;
    attrs[2] = SYSDB_CCACHE_FILE;
    attrs[3] = SYSDB_UIDNUM;
    attrs[4] = SYSDB_GIDNUM;
    attrs[5] = NULL;

    ret = krb5_setup(state, pd, krb5_ctx, &state->kr);
    if (ret != EOK) {
        DEBUG(1, ("krb5_setup failed.\n"));
        goto done;
    }
    kr = state->kr;

    ret = sysdb_get_user_attr(state, be_ctx->sysdb, be_ctx->domain,
                              state->pd->user, attrs, &res);
    if (ret) {
        DEBUG(5, ("sysdb search for upn of user [%s] failed.\n", pd->user));
        state->pam_status = PAM_SYSTEM_ERR;
        state->dp_err = DP_ERR_OK;
        goto done;
    }

    realm = dp_opt_get_cstring(krb5_ctx->opts, KRB5_REALM);
    if (realm == NULL) {
        DEBUG(1, ("Missing Kerberos realm.\n"));
        ret = ENOENT;
        goto done;
    }

    switch (res->count) {
    case 0:
        DEBUG(5, ("No attributes for user [%s] found.\n", pd->user));
        ret = ENOENT;
        goto done;
        break;

    case 1:
        kr->upn = ldb_msg_find_attr_as_string(res->msgs[0], SYSDB_UPN, NULL);
        if (kr->upn == NULL) {
            ret = krb5_get_simple_upn(state, krb5_ctx, pd->user, &kr->upn);
            if (ret != EOK) {
                DEBUG(1, ("krb5_get_simple_upn failed.\n"));
                goto done;
            }
        }

        kr->homedir = ldb_msg_find_attr_as_string(res->msgs[0], SYSDB_HOMEDIR,
                                                  NULL);
        if (kr->homedir == NULL) {
            DEBUG(4, ("Home directory for user [%s] not known.\n", pd->user));
        }

        kr->uid = ldb_msg_find_attr_as_uint64(res->msgs[0], SYSDB_UIDNUM, 0);
        if (kr->uid == 0) {
            DEBUG(4, ("UID for user [%s] not known.\n", pd->user));
            ret = ENOENT;
            goto done;
        }

        kr->gid = ldb_msg_find_attr_as_uint64(res->msgs[0], SYSDB_GIDNUM, 0);
        if (kr->gid == 0) {
            DEBUG(4, ("GID for user [%s] not known.\n", pd->user));
            ret = ENOENT;
            goto done;
        }

        ccache_file = ldb_msg_find_attr_as_string(res->msgs[0],
                                                  SYSDB_CCACHE_FILE,
                                                  NULL);
        if (ccache_file != NULL) {
            ret = check_if_ccache_file_is_used(kr->uid, ccache_file,
                                               &kr->active_ccache_present);
            if (ret != EOK) {
                DEBUG(1, ("check_if_ccache_file_is_used failed.\n"));
                goto done;
            }

            kerr = check_for_valid_tgt(ccache_file, realm, kr->upn,
                                       &kr->valid_tgt_present);
            if (kerr != 0) {
                DEBUG(1, ("check_for_valid_tgt failed.\n"));
                ret = kerr;
                goto done;
            }
        } else {
            kr->active_ccache_present = false;
            kr->valid_tgt_present = false;
            DEBUG(4, ("No ccache file for user [%s] found.\n", pd->user));
        }
        DEBUG(9, ("Ccache_file is [%s] and is %s active and TGT is %s valid.\n",
                  ccache_file ? ccache_file : "not set",
                  kr->active_ccache_present ? "" : "not",
                  kr->valid_tgt_present ? "" : "not"));
        if (ccache_file != NULL) {
            kr->ccname = ccache_file;
            kr->old_ccname = talloc_strdup(kr, ccache_file);
            if (kr->old_ccname == NULL) {
                DEBUG(1, ("talloc_strdup failed.\n"));
                ret = ENOMEM;
                goto done;
            }
        } else {
            kr->ccname = NULL;
            kr->old_ccname = NULL;
        }
        break;

    default:
        DEBUG(1, ("User search for (%s) returned > 1 results!\n", pd->user));
        ret = EINVAL;
        goto done;
        break;
    }

    kr->srv = NULL;
    kr->kpasswd_srv = NULL;
    subreq = be_resolve_server_send(state, state->ev, state->be_ctx,
                                    krb5_ctx->service->name);
    if (subreq == NULL) {
        DEBUG(1, ("be_resolve_server_send failed.\n"));
        ret = ENOMEM;
        goto done;
    }

    tevent_req_set_callback(subreq, krb5_resolve_kdc_done, req);

    return req;

done:
    if (ret == EOK) {
        tevent_req_done(req);
    } else {
        tevent_req_error(req, ret);
    }
    tevent_req_post(req, state->ev);
    return req;
}