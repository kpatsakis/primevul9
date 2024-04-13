static int converse(const pam_handle_t *pamh,
		    int nargs,
		    struct pam_message **message,
		    struct pam_response **response)
{
	int retval;
	struct pam_conv *conv;

	retval = pam_get_item(pamh, PAM_CONV, (const void **) &conv);
	if (retval == PAM_SUCCESS) {
		retval = conv->conv(nargs,
				    (const struct pam_message **)message,
				    response, conv->appdata_ptr);
	}

	return retval; /* propagate error status */
}