int LUKS2_hdr_write_force(struct crypt_device *cd, struct luks2_hdr *hdr)
{
	if (hdr_cleanup_and_validate(cd, hdr))
		return -EINVAL;

	return LUKS2_disk_hdr_write(cd, hdr, crypt_metadata_device(cd), false);
}