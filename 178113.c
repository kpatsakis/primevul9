int LUKS2_hdr_labels(struct crypt_device *cd, struct luks2_hdr *hdr,
		     const char *label, const char *subsystem, int commit)
{
	//FIXME: check if the labels are the same and skip this.

	memset(hdr->label, 0, LUKS2_LABEL_L);
	if (label)
		strncpy(hdr->label, label, LUKS2_LABEL_L-1);

	memset(hdr->subsystem, 0, LUKS2_LABEL_L);
	if (subsystem)
		strncpy(hdr->subsystem, subsystem, LUKS2_LABEL_L-1);

	return commit ? LUKS2_hdr_write(cd, hdr) : 0;
}