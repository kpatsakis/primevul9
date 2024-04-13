int am_check_permissions(request_rec *r, am_cache_entry_t *session)
{
    am_dir_cfg_rec *dir_cfg;
    int i, j;
    int skip_or = 0;
    const apr_array_header_t *backrefs = NULL;

    dir_cfg = am_get_dir_cfg(r);

    /* Iterate over all cond-directives */
    for (i = 0; i < dir_cfg->cond->nelts; i++) {
        const am_cond_t *ce;
        const char *value = NULL;
        int match = 0;

        ce = &((am_cond_t *)(dir_cfg->cond->elts))[i];

        /*
         * Rule with ignore flog?
         */
        if (ce->flags & AM_COND_FLAG_IGN)
            continue;

        /* 
         * We matched a [OR] rule, skip the next rules
         * until we have one without [OR]. 
         */
        if (skip_or) {
            if (!(ce->flags & AM_COND_FLAG_OR))
                skip_or = 0;

             ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                           "Skip %s, [OR] rule matched previously",
                           ce->directive);
            continue;
        }
        
        /* 
         * look for a match on each value for this attribute, 
         * stop on first match.
         */
        for (j = 0; (j < session->size) && !match; j++) {
            const char *varname = NULL;

            /*
             * if MAP flag is set, check for remapped 
             * attribute name with mellonSetEnv
             */
            if (ce->flags & AM_COND_FLAG_MAP)
                varname = apr_hash_get(dir_cfg->envattr, 
                                       am_cache_entry_get_string(session,
                                                    &session->env[j].varname),
                                       APR_HASH_KEY_STRING);

            /*
             * Otherwise or if not found, use the attribute name
             * sent by the IdP.
             */
            if (varname == NULL)
                varname = am_cache_entry_get_string(session,
                                                    &session->env[j].varname);
                      
            if (strcmp(varname, ce->varname) != 0)
                    continue;

            value = am_cache_entry_get_string(session, &session->env[j].value);

            /*
             * Substiture backrefs if available
             */
            if ((ce->flags & AM_COND_FLAG_FSTR) && (backrefs != NULL))
                ce = am_cond_substitue(r, ce, backrefs);

            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                          "Evaluate %s vs \"%s\"", 
                          ce->directive, value);
    
            if (value == NULL) {
                 match = 0;          /* can not happen */

            } else if (ce->flags & (AM_COND_FLAG_REG|AM_COND_FLAG_REF)) {
                 int nsub = ce->regex->re_nsub + 1;
                 ap_regmatch_t *regmatch;

                 regmatch = (ap_regmatch_t *)apr_palloc(r->pool, 
                            nsub * sizeof(*regmatch));

                 match = !ap_regexec(ce->regex, value, nsub, regmatch, 0);
                 if (match)
                     backrefs = am_cond_backrefs(r, ce, value, regmatch);

            } else if (ce->flags & AM_COND_FLAG_REG) {
                 match = !ap_regexec(ce->regex, value, 0, NULL, 0);

            } else if (ce->flags & (AM_COND_FLAG_SUB|AM_COND_FLAG_NC)) {
                 match = (strcasestr(ce->str, value) != NULL);

            } else if (ce->flags & AM_COND_FLAG_SUB) {
                 match = (strstr(ce->str, value) != NULL);

            } else if (ce->flags & AM_COND_FLAG_NC) {
                 match = !strcasecmp(ce->str, value);

            } else {
                 match = !strcmp(ce->str, value);
            }
        }

        if (ce->flags & AM_COND_FLAG_NOT)
            match = !match;

        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r,
                      "%s: %smatch", ce->directive,
                      (match == 0) ? "no ": "");

        /*
         * If no match, we stop here, except if it is an [OR] condition
         */
        if (!match & !(ce->flags & AM_COND_FLAG_OR)) {
            ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r,
                          "Client failed to match %s",
                          ce->directive);
            return HTTP_FORBIDDEN;
        }

        /*
         * Match on [OR] condition means we skip until a rule
         * without [OR], 
         */
        if (match && (ce->flags & AM_COND_FLAG_OR))
            skip_or = 1;
    }

    return OK;
}