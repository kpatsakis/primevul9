int LUKS2_hdr_read(struct crypt_device *cd, struct luks2_hdr *hdr, int repair)
{
	int r;

	r = device_read_lock(cd, crypt_metadata_device(cd));
	if (r) {
		log_err(cd, _("Failed to acquire read lock on device %s."),
			device_path(crypt_metadata_device(cd)));
		return r;
	}

	r = LUKS2_disk_hdr_read(cd, hdr, crypt_metadata_device(cd), 1, !repair);
	if (r == -EAGAIN) {
		/* unlikely: auto-recovery is required and failed due to read lock being held */
		device_read_unlock(cd, crypt_metadata_device(cd));

		/* Do not use LUKS2_device_write lock. Recovery. */
		r = device_write_lock(cd, crypt_metadata_device(cd));
		if (r < 0) {
			log_err(cd, _("Failed to acquire write lock on device %s."),
				device_path(crypt_metadata_device(cd)));
			return r;
		}

		r = LUKS2_disk_hdr_read(cd, hdr, crypt_metadata_device(cd), 1, !repair);

		device_write_unlock(cd, crypt_metadata_device(cd));
	} else
		device_read_unlock(cd, crypt_metadata_device(cd));

	return r;
}