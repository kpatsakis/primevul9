adv_error adv_png_write_signature(adv_fz* f, unsigned* count)
{
	if (fzwrite(PNG_Signature, 8, 1, f) != 1) {
		error_set("Error writing the signature");
		return -1;
	}

	if (count)
		*count += 8;

	return 0;
}