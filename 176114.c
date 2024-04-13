adv_error adv_png_read_signature(adv_fz* f)
{
	unsigned char signature[8];

	if (fzread(signature, 8, 1, f) != 1) {
		error_set("Error reading the signature");
		return -1;
	}

	if (memcmp(signature, PNG_Signature, 8)!=0) {
		error_set("Invalid PNG signature");
		return -1;
	}

	return 0;
}