static int krb5_save_ccname(TALLOC_CTX *mem_ctx,
                            struct sysdb_ctx *sysdb,
                            struct sss_domain_info *domain,
                            const char *name,
                            const char *ccname)
{
    TALLOC_CTX *tmpctx;
    struct sysdb_attrs *attrs;
    int ret;

    if (name == NULL || ccname == NULL) {
        DEBUG(1, ("Missing user or ccache name.\n"));
        return EINVAL;
    }

    DEBUG(9, ("Save ccname [%s] for user [%s].\n", ccname, name));

    tmpctx = talloc_new(mem_ctx);
    if (!tmpctx) {
        return ENOMEM;
    }

    attrs = sysdb_new_attrs(mem_ctx);
    if (!attrs) {
        ret = ENOMEM;
        goto done;
    }

    ret = sysdb_attrs_add_string(attrs, SYSDB_CCACHE_FILE, ccname);
    if (ret != EOK) {
        DEBUG(1, ("sysdb_attrs_add_string failed.\n"));
        goto done;
    }

    ret = sysdb_transaction_start(sysdb);
    if (ret != EOK) {
        DEBUG(6, ("Error %d starting transaction (%s)\n", ret, strerror(ret)));
        goto done;
    }

    ret = sysdb_set_user_attr(tmpctx, sysdb,
                              domain, name, attrs, SYSDB_MOD_REP);
    if (ret != EOK) {
        DEBUG(6, ("Error: %d (%s)\n", ret, strerror(ret)));
        sysdb_transaction_cancel(sysdb);
        goto done;
    }

    ret = sysdb_transaction_commit(sysdb);
    if (ret != EOK) {
        DEBUG(1, ("Failed to commit transaction!\n"));
    }

done:
    talloc_zfree(tmpctx);
    return ret;
}