static errno_t handle_missing_pvt(TALLOC_CTX *mem_ctx,
                                  struct tevent_context *ev,
                                  struct sdap_options *opts,
                                  const char *orig_dn,
                                  int timeout,
                                  const char *username,
                                  struct sdap_handle *sh,
                                  struct tevent_req *req,
                                  tevent_req_fn callback)
{
    struct tevent_req *subreq = NULL;
    errno_t ret;

    if (sh != NULL) {
        /*  plain LDAP provider already has a sdap_handle */
        subreq = sdap_get_ad_tokengroups_send(mem_ctx, ev, opts, sh, username,
                                              orig_dn, timeout);
        if (subreq == NULL) {
            ret = ENOMEM;
            tevent_req_error(req, ret);
            goto done;
        }

        tevent_req_set_callback(subreq, callback, req);
        ret = EOK;
        goto done;

    } else {
        ret = EINVAL;
        goto done;
    }

done:
    return ret;
}