int LUKS2_hdr_restore(struct crypt_device *cd, struct luks2_hdr *hdr,
		     const char *backup_file)
{
	struct device *backup_device, *device = crypt_metadata_device(cd);
	int r, fd, devfd = -1, diff_uuid = 0;
	ssize_t ret, buffer_size = 0;
	char *buffer = NULL, msg[1024];
	struct luks2_hdr hdr_file;
	struct luks2_hdr tmp_hdr = {};
	uint32_t reqs = 0;

	r = device_alloc(cd, &backup_device, backup_file);
	if (r < 0)
		return r;

	r = device_read_lock(cd, backup_device);
	if (r) {
		log_err(cd, _("Failed to acquire read lock on device %s."),
			device_path(backup_device));
		device_free(cd, backup_device);
		return r;
	}

	r = LUKS2_disk_hdr_read(cd, &hdr_file, backup_device, 0, 0);
	device_read_unlock(cd, backup_device);
	device_free(cd, backup_device);

	if (r < 0) {
		log_err(cd, _("Backup file does not contain valid LUKS header."));
		goto out;
	}

	/* do not allow header restore from backup with unmet requirements */
	if (LUKS2_unmet_requirements(cd, &hdr_file, CRYPT_REQUIREMENT_ONLINE_REENCRYPT, 1)) {
		log_err(cd, _("Forbidden LUKS2 requirements detected in backup %s."),
			backup_file);
		r = -ETXTBSY;
		goto out;
	}

	buffer_size = LUKS2_hdr_and_areas_size(&hdr_file);
	buffer = crypt_safe_alloc(buffer_size);
	if (!buffer) {
		r = -ENOMEM;
		goto out;
	}

	fd = open(backup_file, O_RDONLY);
	if (fd == -1) {
		log_err(cd, _("Cannot open header backup file %s."), backup_file);
		r = -EINVAL;
		goto out;
	}

	ret = read_buffer(fd, buffer, buffer_size);
	close(fd);
	if (ret < buffer_size) {
		log_err(cd, _("Cannot read header backup file %s."), backup_file);
		r = -EIO;
		goto out;
	}

	r = LUKS2_hdr_read(cd, &tmp_hdr, 0);
	if (r == 0) {
		log_dbg(cd, "Device %s already contains LUKS2 header, checking UUID and requirements.", device_path(device));
		r = LUKS2_config_get_requirements(cd, &tmp_hdr, &reqs);
		if (r)
			goto out;

		if (memcmp(tmp_hdr.uuid, hdr_file.uuid, LUKS2_UUID_L))
			diff_uuid = 1;

		if (!reqs_reencrypt(reqs)) {
			log_dbg(cd, "Checking LUKS2 header size and offsets.");
			if (LUKS2_get_data_offset(&tmp_hdr) != LUKS2_get_data_offset(&hdr_file)) {
				log_err(cd, _("Data offset differ on device and backup, restore failed."));
				r = -EINVAL;
				goto out;
			}
			/* FIXME: what could go wrong? Erase if we're fine with consequences */
			if (buffer_size != (ssize_t) LUKS2_hdr_and_areas_size(&tmp_hdr)) {
				log_err(cd, _("Binary header with keyslot areas size differ on device and backup, restore failed."));
				r = -EINVAL;
				goto out;
			}
		}
	}

	r = snprintf(msg, sizeof(msg), _("Device %s %s%s%s%s"), device_path(device),
		     r ? _("does not contain LUKS2 header. Replacing header can destroy data on that device.") :
			 _("already contains LUKS2 header. Replacing header will destroy existing keyslots."),
		     diff_uuid ? _("\nWARNING: real device header has different UUID than backup!") : "",
		     reqs_unknown(reqs) ? _("\nWARNING: unknown LUKS2 requirements detected in real device header!"
					    "\nReplacing header with backup may corrupt the data on that device!") : "",
		     reqs_reencrypt(reqs) ? _("\nWARNING: Unfinished offline reencryption detected on the device!"
					      "\nReplacing header with backup may corrupt data.") : "");
	if (r < 0 || (size_t) r >= sizeof(msg)) {
		r = -ENOMEM;
		goto out;
	}

	if (!crypt_confirm(cd, msg)) {
		r = -EINVAL;
		goto out;
	}

	log_dbg(cd, "Storing backup of header (%zu bytes) to device %s.", buffer_size, device_path(device));

	/* Do not use LUKS2_device_write lock for checking sequence id on restore */
	r = device_write_lock(cd, device);
	if (r < 0) {
		log_err(cd, _("Failed to acquire write lock on device %s."),
			device_path(device));
		goto out;
	}

	devfd = device_open_locked(cd, device, O_RDWR);
	if (devfd < 0) {
		if (errno == EACCES)
			log_err(cd, _("Cannot write to device %s, permission denied."),
				device_path(device));
		else
			log_err(cd, _("Cannot open device %s."), device_path(device));
		device_write_unlock(cd, device);
		r = -EINVAL;
		goto out;
	}

	if (write_lseek_blockwise(devfd, device_block_size(cd, device),
			    device_alignment(device), buffer, buffer_size, 0) < buffer_size)
		r = -EIO;
	else
		r = 0;

	device_write_unlock(cd, device);
out:
	LUKS2_hdr_free(cd, hdr);
	LUKS2_hdr_free(cd, &hdr_file);
	LUKS2_hdr_free(cd, &tmp_hdr);
	crypt_safe_memzero(&hdr_file, sizeof(hdr_file));
	crypt_safe_memzero(&tmp_hdr, sizeof(tmp_hdr));
	crypt_safe_free(buffer);

	device_sync(cd, device);

	return r;
}