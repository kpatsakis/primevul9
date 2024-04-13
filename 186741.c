static void krb5_child_done(struct tevent_req *subreq)
{
    struct tevent_req *req = tevent_req_callback_data(subreq, struct tevent_req);
    struct krb5_auth_state *state = tevent_req_data(req, struct krb5_auth_state);

    struct krb5child_req *kr = state->kr;
    struct pam_data *pd = state->pd;
    int ret;
    uint8_t *buf = NULL;
    ssize_t len = -1;
    ssize_t pref_len;
    size_t p;
    int32_t msg_status;
    int32_t msg_type;
    int32_t msg_len;
    int64_t time_data;
    struct tgt_times tgtt;

    memset(&tgtt, 0, sizeof(tgtt));

    ret = handle_child_recv(subreq, pd, &buf, &len);
    talloc_zfree(subreq);
    if (ret != EOK) {
        DEBUG(1, ("child failed (%d [%s])\n", ret, strerror(ret)));
        if (ret == ETIMEDOUT) {
            if (krb5_next_server(req) == NULL) {
                tevent_req_error(req, ENOMEM);
            }
        } else {
            tevent_req_error(req, ret);
        }
        return;
    }

    /* A buffer with the following structure is expected.
     * int32_t status of the request (required)
     * message (zero or more)
     *
     * A message consists of:
     * int32_t type of the message
     * int32_t length of the following data
     * uint8_t[len] data
     */

    if ((size_t) len < sizeof(int32_t)) {
        DEBUG(1, ("message too short.\n"));
        ret = EINVAL;
        goto done;
    }

    p=0;
    SAFEALIGN_COPY_INT32(&msg_status, buf+p, &p);

    while (p < len) {
        SAFEALIGN_COPY_INT32(&msg_type, buf+p, &p);
        SAFEALIGN_COPY_INT32(&msg_len, buf+p, &p);

        DEBUG(9, ("child response [%d][%d][%d].\n", msg_status, msg_type,
                                                    msg_len));

        if ((p + msg_len) > len) {
            DEBUG(1, ("message format error [%d] > [%d].\n", p+msg_len, len));
            ret = EINVAL;
            goto done;
        }

        /* We need to save the name of the credential cache file. To find it
         * we check if the data part of a message starts with
         * CCACHE_ENV_NAME"=". pref_len also counts the trailing '=' because
         * sizeof() counts the trailing '\0' of a string. */
        pref_len = sizeof(CCACHE_ENV_NAME);
        if (msg_len > pref_len &&
            strncmp((const char *) &buf[p], CCACHE_ENV_NAME"=", pref_len) == 0) {
            kr->ccname = talloc_strndup(kr, (char *) &buf[p+pref_len],
                                        msg_len-pref_len);
            if (kr->ccname == NULL) {
                DEBUG(1, ("talloc_strndup failed.\n"));
                ret = ENOMEM;
                goto done;
            }
        }

        if (msg_type == SSS_KRB5_INFO_TGT_LIFETIME &&
            msg_len == 4*sizeof(int64_t)) {
            SAFEALIGN_COPY_INT64(&time_data, buf+p, NULL);
            tgtt.authtime = int64_to_time_t(time_data);
            SAFEALIGN_COPY_INT64(&time_data, buf+p+sizeof(int64_t), NULL);
            tgtt.starttime = int64_to_time_t(time_data);
            SAFEALIGN_COPY_INT64(&time_data, buf+p+2*sizeof(int64_t), NULL);
            tgtt.endtime = int64_to_time_t(time_data);
            SAFEALIGN_COPY_INT64(&time_data, buf+p+3*sizeof(int64_t), NULL);
            tgtt.renew_till = int64_to_time_t(time_data);
            DEBUG(7, ("TGT times are [%d][%d][%d][%d].\n", tgtt.authtime,
                      tgtt.starttime, tgtt.endtime, tgtt.renew_till));
        }

        ret = pam_add_response(pd, msg_type, msg_len, &buf[p]);
        if (ret != EOK) {
            /* This is not a fatal error */
            DEBUG(1, ("pam_add_response failed.\n"));
        }
        p += msg_len;

        if ((p < len) && (p + 2*sizeof(int32_t) >= len)) {
            DEBUG(1, ("The remainder of the message is too short.\n"));
            ret = EINVAL;
            goto done;
        }
    }

    /* If the child request failed, but did not return an offline error code,
     * return with the status */
    if (msg_status != PAM_SUCCESS && msg_status != PAM_AUTHINFO_UNAVAIL &&
        msg_status != PAM_AUTHTOK_LOCK_BUSY) {
        state->pam_status = msg_status;
        state->dp_err = DP_ERR_OK;
        ret = EOK;
        goto done;
    } else {
        state->pam_status = msg_status;
    }

    /* If the child request was successful and we run the first pass of the
     * change password request just return success. */
    if (msg_status == PAM_SUCCESS && pd->cmd == SSS_PAM_CHAUTHTOK_PRELIM) {
        state->pam_status = PAM_SUCCESS;
        state->dp_err = DP_ERR_OK;
        ret = EOK;
        goto done;
    }

    /* if using a dedicated kpasswd server.. */
    if (kr->kpasswd_srv != NULL) {
        /* ..which is unreachable by now.. */
        if (msg_status == PAM_AUTHTOK_LOCK_BUSY) {
            fo_set_port_status(kr->kpasswd_srv, PORT_NOT_WORKING);
            /* ..try to resolve next kpasswd server */
            if (krb5_next_kpasswd(req) == NULL) {
                tevent_req_error(req, ENOMEM);
            }
            return;
        } else {
            fo_set_port_status(kr->kpasswd_srv, PORT_WORKING);
        }
    }

    /* if the KDC for auth (PAM_AUTHINFO_UNAVAIL) or
     * chpass (PAM_AUTHTOK_LOCK_BUSY) was not available while using KDC
     * also for chpass operation... */
    if (msg_status == PAM_AUTHINFO_UNAVAIL ||
        (kr->kpasswd_srv == NULL && msg_status == PAM_AUTHTOK_LOCK_BUSY)) {
        if (kr->srv != NULL) {
            fo_set_port_status(kr->srv, PORT_NOT_WORKING);
            /* ..try to resolve next KDC */
            if (krb5_next_kdc(req) == NULL) {
                tevent_req_error(req, ENOMEM);
            }
            return;
        }
    } else if (kr->srv != NULL) {
        fo_set_port_status(kr->srv, PORT_WORKING);
    }

    /* Now only a successful authentication or password change is left.
     *
     * We expect that one of the messages in the received buffer contains
     * the name of the credential cache file. */
    if (kr->ccname == NULL) {
        DEBUG(1, ("Missing ccache name in child response.\n"));
        ret = EINVAL;
        goto done;
    }

    if (kr->old_ccname != NULL) {
        ret = safe_remove_old_ccache_file(kr->old_ccname, kr->ccname);
        if (ret != EOK) {
            DEBUG(1, ("Failed to remove old ccache file [%s], please remove it manually.\n"));
        }
    }

    ret = krb5_save_ccname(state, state->be_ctx->sysdb,
                           state->be_ctx->domain,
                           pd->user, kr->ccname);
    if (ret) {
        DEBUG(1, ("krb5_save_ccname_send failed.\n"));
        goto done;
    }

    if (msg_status == PAM_SUCCESS &&
        dp_opt_get_int(kr->krb5_ctx->opts, KRB5_RENEW_INTERVAL) > 0 &&
        (pd->cmd == SSS_PAM_AUTHENTICATE || pd->cmd == SSS_CMD_RENEW ||
         pd->cmd == SSS_PAM_CHAUTHTOK) &&
        tgtt.renew_till > tgtt.endtime && kr->ccname != NULL) {
        DEBUG(7, ("Adding [%s] for automatic renewal.\n", kr->ccname));
        ret = add_tgt_to_renew_table(kr->krb5_ctx, kr->ccname, &tgtt, pd,
                                     kr->upn);
        if (ret != EOK) {
            DEBUG(1, ("add_tgt_to_renew_table failed, "
                      "automatic renewal not possible.\n"));
        }
    }

    krb5_save_ccname_done(req);

    return;

done:
    if (ret == EOK) {
        tevent_req_done(req);
    } else {
        tevent_req_error(req, ret);
    }
}