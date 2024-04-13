adv_error adv_png_read_chunk(adv_fz* f, unsigned char** data, unsigned* size, unsigned* type)
{
	unsigned char cl[4];
	unsigned char ct[4];
	unsigned char cc[4];

	if (fzread(cl, 4, 1, f) != 1) {
		error_set("Error reading the chunk size");
		goto err;
	}

	*size = be_uint32_read(cl);

	if (fzread(ct, 4, 1, f) != 1) {
		error_set("Error reading the chunk type");
		goto err;
	}

	*type = be_uint32_read(ct);

	if (*size) {
		*data = malloc(*size);
		if (!*data) {
			error_set("Low memory");
			goto err;
		}

		if (fzread(*data, *size, 1, f) != 1) {
			error_set("Error reading the chunk data");
			goto err_data;
		}
	} else {
		*data = 0;
	}

	if (fzread(cc, 4, 1, f) != 1) {
		error_set("Error reading the chunk crc");
		goto err_data;
	}

	return 0;
err_data:
	free(*data);
err:
	return -1;
}