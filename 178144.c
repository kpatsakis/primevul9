int LUKS2_hdr_uuid(struct crypt_device *cd, struct luks2_hdr *hdr, const char *uuid)
{
	uuid_t partitionUuid;

	if (uuid && uuid_parse(uuid, partitionUuid) == -1) {
		log_err(cd, _("Wrong LUKS UUID format provided."));
		return -EINVAL;
	}
	if (!uuid)
		uuid_generate(partitionUuid);

	uuid_unparse(partitionUuid, hdr->uuid);

	return LUKS2_hdr_write(cd, hdr);
}