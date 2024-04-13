void modsecurity_child_init(msc_engine *msce) {
    /* Need to call this once per process before any other XML calls. */
    xmlInitParser();

    if (msce->auditlog_lock != NULL) {
        apr_status_t rc = apr_global_mutex_child_init(&msce->auditlog_lock, NULL, msce->mp);
        if (rc != APR_SUCCESS) {
            // ap_log_error(APLOG_MARK, APLOG_ERR, rs, s, "Failed to child-init auditlog mutex");
        }
    }

    if (msce->geo_lock != NULL) {
        apr_status_t rc = apr_global_mutex_child_init(&msce->geo_lock, NULL, msce->mp);
        if (rc != APR_SUCCESS) {
            // ap_log_error(APLOG_MARK, APLOG_ERR, rs, s, "Failed to child-init geo mutex");
        }
    }

}