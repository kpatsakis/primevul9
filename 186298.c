static void php_session_rfc1867_update(php_session_rfc1867_progress *progress, int force_update) /* {{{ */
{
	if (!force_update) {
		if (Z_LVAL_P(progress->post_bytes_processed) < progress->next_update) {
			return;
		}
#ifdef HAVE_GETTIMEOFDAY
		if (PS(rfc1867_min_freq) > 0.0) {
			struct timeval tv = {0};
			double dtv;
			gettimeofday(&tv, NULL);
			dtv = (double) tv.tv_sec + tv.tv_usec / 1000000.0;
			if (dtv < progress->next_update_time) {
				return;
			}
			progress->next_update_time = dtv + PS(rfc1867_min_freq);
		}
#endif
		progress->next_update = Z_LVAL_P(progress->post_bytes_processed) + progress->update_step;
	}

	php_session_initialize();
	PS(session_status) = php_session_active;
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);

		progress->cancel_upload |= php_check_cancel_upload(progress);
		Z_TRY_ADDREF(progress->data);
		zend_hash_update(Z_ARRVAL_P(sess_var), progress->key.s, &progress->data);
	}
	php_session_flush(1);
} /* }}} */