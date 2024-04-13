static apr_status_t modsecurity_process_phase_response_headers(modsec_rec *msr) {
    apr_time_t time_before;
    apr_status_t rc = 0;
    
    if (msr->allow_scope == ACTION_ALLOW) {
        if (msr->txcfg->debuglog_level >= 4) {
            msr_log(msr, 4, "Skipping phase RESPONSE_HEADERS (allow used).");
        }
        
        return 0;
    } else {
        if (msr->txcfg->debuglog_level >= 4) {
            msr_log(msr, 4, "Starting phase RESPONSE_HEADERS.");
        }
    }
    
    time_before = apr_time_now();

    if (msr->txcfg->ruleset != NULL) {
        rc = msre_ruleset_process_phase(msr->txcfg->ruleset, msr);
    }
    
    msr->time_phase3 = apr_time_now() - time_before;

    return rc;
}