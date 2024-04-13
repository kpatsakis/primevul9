static apr_status_t modsecurity_process_phase_logging(modsec_rec *msr) {
    apr_time_t time_before, time_after;
    
    if (msr->txcfg->debuglog_level >= 4) {
        msr_log(msr, 4, "Starting phase LOGGING.");
    }
    
    time_before = apr_time_now();

    if (msr->txcfg->ruleset != NULL) {
        msre_ruleset_process_phase(msr->txcfg->ruleset, msr);
    }
    
    modsecurity_persist_data(msr);
    
    time_after = apr_time_now();
    msr->time_phase5 = time_after - time_before;

    /* Is this request relevant for logging purposes? */
    if (msr->is_relevant == 0) {
        /* Check the status */
        msr->is_relevant += is_response_status_relevant(msr, msr->r->status);

        /* If we processed two requests and statuses are different then
         * check the other status too.
         */
        if (msr->r_early->status != msr->r->status) {
            msr->is_relevant += is_response_status_relevant(msr, msr->r_early->status);
        }
    }

    /* Figure out if we want to keep the files (if there are any, of course). */
    if ((msr->txcfg->upload_keep_files == KEEP_FILES_ON)
        || ((msr->txcfg->upload_keep_files == KEEP_FILES_RELEVANT_ONLY)&&(msr->is_relevant)))
    {
        msr->upload_remove_files = 0;
    } else {
        msr->upload_remove_files = 1;
    }

    /* Are we configured for audit logging? */
    switch(msr->txcfg->auditlog_flag) {
        case AUDITLOG_OFF :
            if (msr->txcfg->debuglog_level >= 4) {
                msr_log(msr, 4, "Audit log: Not configured to run for this request.");
            }
            
            return DECLINED;
            break;

        case AUDITLOG_RELEVANT :
            if (msr->is_relevant == 0) {
                if (msr->txcfg->debuglog_level >= 4) {
                    msr_log(msr, 4, "Audit log: Ignoring a non-relevant request.");
                }
                
                return DECLINED;
            }
            break;

        case AUDITLOG_ON :
            /* All right, do nothing */
            break;

        default :
            msr_log(msr, 1, "Internal error: Could not determine if auditing is needed, so forcing auditing.");
            break;
    }

    /* Invoke the Audit logger */
    if (msr->txcfg->debuglog_level >= 4) {
        msr_log(msr, 4, "Audit log: Logging this transaction.");
    }

    sec_audit_logger(msr);
    
    msr->time_logging = apr_time_now() - time_after;    

    return 0;
}