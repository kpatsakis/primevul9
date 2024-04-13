static int wbc_error_to_pam_error(wbcErr status)
{
	switch (status) {
		case WBC_ERR_SUCCESS:
			return PAM_SUCCESS;
		case WBC_ERR_NOT_IMPLEMENTED:
			return PAM_SERVICE_ERR;
		case WBC_ERR_UNKNOWN_FAILURE:
			break;
		case WBC_ERR_NO_MEMORY:
			return PAM_BUF_ERR;
		case WBC_ERR_INVALID_SID:
		case WBC_ERR_INVALID_PARAM:
			break;
		case WBC_ERR_WINBIND_NOT_AVAILABLE:
			return PAM_AUTHINFO_UNAVAIL;
		case WBC_ERR_DOMAIN_NOT_FOUND:
			return PAM_AUTHINFO_UNAVAIL;
		case WBC_ERR_INVALID_RESPONSE:
			return PAM_BUF_ERR;
		case WBC_ERR_NSS_ERROR:
			return PAM_USER_UNKNOWN;
		case WBC_ERR_AUTH_ERROR:
			return PAM_AUTH_ERR;
		case WBC_ERR_UNKNOWN_USER:
			return PAM_USER_UNKNOWN;
		case WBC_ERR_UNKNOWN_GROUP:
			return PAM_USER_UNKNOWN;
		case WBC_ERR_PWD_CHANGE_FAILED:
			break;
	}

	/* be paranoid */
	return PAM_AUTH_ERR;
}