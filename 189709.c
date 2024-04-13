apr_status_t modsecurity_process_phase(modsec_rec *msr, unsigned int phase) {
    /* Check if we should run. */
    if ((msr->was_intercepted)&&(phase != PHASE_LOGGING)) {
        if (msr->txcfg->debuglog_level >= 4) {
            msr_log(msr, 4, "Skipping phase %d as request was already intercepted.", phase);
        }
        
        return 0;
    }

    /* Do not process the same phase twice. */
    if (msr->phase >= phase) {
        if (msr->txcfg->debuglog_level >= 4) {
            msr_log(msr, 4, "Skipping phase %d because it was previously run (at %d now).",
                phase, msr->phase);
        }
        
        return 0;
    }

    msr->phase = phase;

    /* Clear out the transformation cache at the start of each phase */
    if (msr->txcfg->cache_trans == MODSEC_CACHE_ENABLED) {
        if (msr->tcache) {
            apr_hash_index_t *hi;
            void *dummy;
            apr_table_t *tab;
            const void *key;
            apr_ssize_t klen;
            #ifdef CACHE_DEBUG
            apr_pool_t *mp = msr->msc_rule_mptmp;
            const apr_array_header_t *ctarr;
            const apr_table_entry_t *ctelts;
            msre_cache_rec *rec;
            int cn = 0;
            int ri;
            #else
            apr_pool_t *mp = msr->mp;
            #endif

            for (hi = apr_hash_first(mp, msr->tcache); hi; hi = apr_hash_next(hi)) {
                apr_hash_this(hi, &key, &klen, &dummy);
                tab = (apr_table_t *)dummy;

                if (tab == NULL) continue;

                #ifdef CACHE_DEBUG
                /* Dump the cache out as we clear */
                ctarr = apr_table_elts(tab);
                ctelts = (const apr_table_entry_t*)ctarr->elts;
                for (ri = 0; ri < ctarr->nelts; ri++) {
                    cn++;
                    rec = (msre_cache_rec *)ctelts[ri].val;
                    if (rec->changed) {
                        if (msr->txcfg->debuglog_level >= 9) {
                            msr_log(msr, 9, "CACHE: %5d) hits=%d key=%pp %x;%s=\"%s\" (%pp - %pp)",
                                cn, rec->hits, key, rec->num, rec->path, log_escape_nq_ex(mp, rec->val, rec->val_len),
                                rec->val, rec->val + rec->val_len);
                        }
                    }
                    else {
                        if (msr->txcfg->debuglog_level >= 9) {
                            msr_log(msr, 9, "CACHE: %5d) hits=%d key=%pp %x;%s=<no change>",
                                cn, rec->hits, key, rec->num, rec->path);
                        }
                    }
                }
                #endif

                apr_table_clear(tab);
                apr_hash_set(msr->tcache, key, klen, NULL);
            }

            if (msr->txcfg->debuglog_level >= 9) {
                msr_log(msr, 9, "Cleared transformation cache for phase %d", msr->phase);
            }
        }

        msr->tcache_items = 0;
        msr->tcache = apr_hash_make(msr->mp);
        if (msr->tcache == NULL) return -1;
    }

    switch(phase) {
        case 1 :
            return modsecurity_process_phase_request_headers(msr);
        case 2 :
            return modsecurity_process_phase_request_body(msr);
        case 3 :
            return modsecurity_process_phase_response_headers(msr);
        case 4 :
            return modsecurity_process_phase_response_body(msr);
        case 5 :
            return modsecurity_process_phase_logging(msr);
        default :
            msr_log(msr, 1, "Invalid processing phase: %d", msr->phase);
            break;
    }

    return -1;
}