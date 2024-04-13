int LUKS2_unmet_requirements(struct crypt_device *cd, struct luks2_hdr *hdr, uint32_t reqs_mask, int quiet)
{
	uint32_t reqs;
	int r = LUKS2_config_get_requirements(cd, hdr, &reqs);

	if (r) {
		if (!quiet)
			log_err(cd, _("Failed to read LUKS2 requirements."));
		return r;
	}

	/* do not mask unknown requirements check */
	if (reqs_unknown(reqs)) {
		if (!quiet)
			log_err(cd, _("Unmet LUKS2 requirements detected."));
		return -ETXTBSY;
	}

	/* mask out permitted requirements */
	reqs &= ~reqs_mask;

	if (reqs_reencrypt(reqs) && !quiet)
		log_err(cd, _("Operation incompatible with device marked for legacy reencryption. Aborting."));
	if (reqs_reencrypt_online(reqs) && !quiet)
		log_err(cd, _("Operation incompatible with device marked for LUKS2 reencryption. Aborting."));

	/* any remaining unmasked requirement fails the check */
	return reqs ? -EINVAL : 0;
}