errno_t krb5_setup(TALLOC_CTX *mem_ctx, struct pam_data *pd,
                   struct krb5_ctx *krb5_ctx, struct krb5child_req **krb5_req)
{
    struct krb5child_req *kr = NULL;

    kr = talloc_zero(mem_ctx, struct krb5child_req);
    if (kr == NULL) {
        DEBUG(1, ("talloc failed.\n"));
        return ENOMEM;
    }
    kr->is_offline = false;
    kr->active_ccache_present = true;
    kr->run_as_user = true;
    talloc_set_destructor((TALLOC_CTX *) kr, krb5_cleanup);

    kr->pd = pd;
    kr->krb5_ctx = krb5_ctx;

    *krb5_req = kr;

    return EOK;
}