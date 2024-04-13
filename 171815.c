static const char *_pam_error_code_str(int err)
{
	switch (err) {
		case PAM_SUCCESS:
			return "PAM_SUCCESS";
		case PAM_OPEN_ERR:
			return "PAM_OPEN_ERR";
		case PAM_SYMBOL_ERR:
			return "PAM_SYMBOL_ERR";
		case PAM_SERVICE_ERR:
			return "PAM_SERVICE_ERR";
		case PAM_SYSTEM_ERR:
			return "PAM_SYSTEM_ERR";
		case PAM_BUF_ERR:
			return "PAM_BUF_ERR";
		case PAM_PERM_DENIED:
			return "PAM_PERM_DENIED";
		case PAM_AUTH_ERR:
			return "PAM_AUTH_ERR";
		case PAM_CRED_INSUFFICIENT:
			return "PAM_CRED_INSUFFICIENT";
		case PAM_AUTHINFO_UNAVAIL:
			return "PAM_AUTHINFO_UNAVAIL";
		case PAM_USER_UNKNOWN:
			return "PAM_USER_UNKNOWN";
		case PAM_MAXTRIES:
			return "PAM_MAXTRIES";
		case PAM_NEW_AUTHTOK_REQD:
			return "PAM_NEW_AUTHTOK_REQD";
		case PAM_ACCT_EXPIRED:
			return "PAM_ACCT_EXPIRED";
		case PAM_SESSION_ERR:
			return "PAM_SESSION_ERR";
		case PAM_CRED_UNAVAIL:
			return "PAM_CRED_UNAVAIL";
		case PAM_CRED_EXPIRED:
			return "PAM_CRED_EXPIRED";
		case PAM_CRED_ERR:
			return "PAM_CRED_ERR";
		case PAM_NO_MODULE_DATA:
			return "PAM_NO_MODULE_DATA";
		case PAM_CONV_ERR:
			return "PAM_CONV_ERR";
		case PAM_AUTHTOK_ERR:
			return "PAM_AUTHTOK_ERR";
		case PAM_AUTHTOK_RECOVER_ERR:
			return "PAM_AUTHTOK_RECOVER_ERR";
		case PAM_AUTHTOK_LOCK_BUSY:
			return "PAM_AUTHTOK_LOCK_BUSY";
		case PAM_AUTHTOK_DISABLE_AGING:
			return "PAM_AUTHTOK_DISABLE_AGING";
		case PAM_TRY_AGAIN:
			return "PAM_TRY_AGAIN";
		case PAM_IGNORE:
			return "PAM_IGNORE";
		case PAM_ABORT:
			return "PAM_ABORT";
		case PAM_AUTHTOK_EXPIRED:
			return "PAM_AUTHTOK_EXPIRED";
#ifdef PAM_MODULE_UNKNOWN
		case PAM_MODULE_UNKNOWN:
			return "PAM_MODULE_UNKNOWN";
#endif
#ifdef PAM_BAD_ITEM
		case PAM_BAD_ITEM:
			return "PAM_BAD_ITEM";
#endif
#ifdef PAM_CONV_AGAIN
		case PAM_CONV_AGAIN:
			return "PAM_CONV_AGAIN";
#endif
#ifdef PAM_INCOMPLETE
		case PAM_INCOMPLETE:
			return "PAM_INCOMPLETE";
#endif
		default:
			return NULL;
	}
}