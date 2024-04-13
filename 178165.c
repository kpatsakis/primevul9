int LUKS2_hdr_backup(struct crypt_device *cd, struct luks2_hdr *hdr,
		     const char *backup_file)
{
	struct device *device = crypt_metadata_device(cd);
	int fd, devfd, r = 0;
	ssize_t hdr_size;
	ssize_t ret, buffer_size;
	char *buffer = NULL;

	hdr_size = LUKS2_hdr_and_areas_size(hdr);
	buffer_size = size_round_up(hdr_size, crypt_getpagesize());

	buffer = crypt_safe_alloc(buffer_size);
	if (!buffer)
		return -ENOMEM;

	log_dbg(cd, "Storing backup of header (%zu bytes).", hdr_size);
	log_dbg(cd, "Output backup file size: %zu bytes.", buffer_size);

	r = device_read_lock(cd, device);
	if (r) {
		log_err(cd, _("Failed to acquire read lock on device %s."),
			device_path(crypt_metadata_device(cd)));
		crypt_safe_free(buffer);
		return r;
	}

	devfd = device_open_locked(cd, device, O_RDONLY);
	if (devfd < 0) {
		device_read_unlock(cd, device);
		log_err(cd, _("Device %s is not a valid LUKS device."), device_path(device));
		crypt_safe_free(buffer);
		return devfd == -1 ? -EINVAL : devfd;
	}

	if (read_lseek_blockwise(devfd, device_block_size(cd, device),
			   device_alignment(device), buffer, hdr_size, 0) < hdr_size) {
		device_read_unlock(cd, device);
		crypt_safe_free(buffer);
		return -EIO;
	}

	device_read_unlock(cd, device);

	fd = open(backup_file, O_CREAT|O_EXCL|O_WRONLY, S_IRUSR);
	if (fd == -1) {
		if (errno == EEXIST)
			log_err(cd, _("Requested header backup file %s already exists."), backup_file);
		else
			log_err(cd, _("Cannot create header backup file %s."), backup_file);
		crypt_safe_free(buffer);
		return -EINVAL;
	}
	ret = write_buffer(fd, buffer, buffer_size);
	close(fd);
	if (ret < buffer_size) {
		log_err(cd, _("Cannot write header backup file %s."), backup_file);
		r = -EIO;
	} else
		r = 0;

	crypt_safe_free(buffer);
	return r;
}