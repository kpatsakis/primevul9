int modsecurity_init(msc_engine *msce, apr_pool_t *mp) {
    apr_status_t rc;

    /* Serial audit log mutext */
    rc = apr_global_mutex_create(&msce->auditlog_lock, NULL, APR_LOCK_DEFAULT, mp);
    if (rc != APR_SUCCESS) {
        //ap_log_error(APLOG_MARK, APLOG_ERR, rv, s, "mod_security: Could not create modsec_auditlog_lock");
        //return HTTP_INTERNAL_SERVER_ERROR;
        return -1;
    }

#if !defined(MSC_TEST)
#ifdef __SET_MUTEX_PERMS
#if AP_SERVER_MAJORVERSION_NUMBER > 1 && AP_SERVER_MINORVERSION_NUMBER > 2
    rc = ap_unixd_set_global_mutex_perms(msce->auditlog_lock);
#else
    rc = unixd_set_global_mutex_perms(msce->auditlog_lock);
#endif
    if (rc != APR_SUCCESS) {
        // ap_log_error(APLOG_MARK, APLOG_ERR, rc, s, "mod_security: Could not set permissions on modsec_auditlog_lock; check User and Group directives");
        // return HTTP_INTERNAL_SERVER_ERROR;
        return -1;
    }
#endif /* SET_MUTEX_PERMS */

    rc = apr_global_mutex_create(&msce->geo_lock, NULL, APR_LOCK_DEFAULT, mp);
    if (rc != APR_SUCCESS) {
        return -1;
    }

#ifdef __SET_MUTEX_PERMS
#if AP_SERVER_MAJORVERSION_NUMBER > 1 && AP_SERVER_MINORVERSION_NUMBER > 2
    rc = ap_unixd_set_global_mutex_perms(msce->geo_lock);
#else
    rc = unixd_set_global_mutex_perms(msce->geo_lock);
#endif
    if (rc != APR_SUCCESS) {
        return -1;
    }
#endif /* SET_MUTEX_PERMS */
#endif

    return 1;
}