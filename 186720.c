sdap_ad_tokengroups_update_members(TALLOC_CTX *mem_ctx,
                                   const char *username,
                                   struct sysdb_ctx *sysdb,
                                   struct sss_domain_info *domain,
                                   char **ldap_groups)
{
    TALLOC_CTX *tmp_ctx = NULL;
    char **sysdb_groups = NULL;
    char **add_groups = NULL;
    char **del_groups = NULL;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "talloc_new() failed\n");
        return ENOMEM;
    }

    /* Get the current sysdb group list for this user so we can update it. */
    ret = get_sysdb_grouplist_dn(tmp_ctx, sysdb, domain,
                                 username, &sysdb_groups);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Could not get the list of groups for "
              "[%s] in the sysdb: [%s]\n", username, strerror(ret));
        goto done;
    }

    /* Find the differences between the sysdb and LDAP lists.
     * Groups in the sysdb only must be removed. */
    ret = diff_string_lists(tmp_ctx, ldap_groups, sysdb_groups,
                            &add_groups, &del_groups, NULL);
    if (ret != EOK) {
        goto done;
    }

    DEBUG(SSSDBG_TRACE_LIBS, "Updating memberships for [%s]\n", username);

    ret = sysdb_update_members_dn(domain->sysdb, domain, username,
                                  SYSDB_MEMBER_USER,
                                  (const char *const *) add_groups,
                                  (const char *const *) del_groups);
    if (ret != EOK) {
        DEBUG(SSSDBG_MINOR_FAILURE, "Membership update failed [%d]: %s\n",
                                     ret, strerror(ret));
        goto done;
    }

done:
    talloc_free(tmp_ctx);
    return ret;
}