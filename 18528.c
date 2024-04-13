generate_signature(cms_context *cms)
{
	int rc = 0;

	if (cms->digests[cms->selected_digest].pe_digest == NULL) {
		cms->log(cms, LOG_ERR, "%s:%s:%d PE digest has not been "
			"allocated", __FILE__, __func__, __LINE__);
		return -1;
	}

	if (content_is_empty(cms->digests[cms->selected_digest].pe_digest->data,
			cms->digests[cms->selected_digest].pe_digest->len)) {
		cms->log(cms, LOG_ERR, "%s:%s:%d PE binary has not been "
			"digested", __FILE__, __func__, __LINE__);
		return -1;
	}

	SECItem sd_der;
	memset(&sd_der, '\0', sizeof(sd_der));
	rc = generate_spc_signed_data(cms, &sd_der);
	if (rc < 0)
		cmsreterr(-1, cms, "could not create signed data");

	memcpy(&cms->newsig, &sd_der, sizeof (cms->newsig));
	cms->newsig.data = malloc(sd_der.len);
	if (!cms->newsig.data)
		cmsreterr(-1, cms, "could not allocate signed data");
	memcpy(cms->newsig.data, sd_der.data, sd_der.len);
	return 0;
}