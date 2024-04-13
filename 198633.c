static int create_key_files(void)
{
	sc_file_t *file;
	int mod_lens[] = { 512, 768, 1024, 2048 };
	int sizes[] = { 163, 243, 323, 643 };
	int size = -1;
	int r;
	size_t i;

	for (i = 0; i < sizeof(mod_lens) / sizeof(int); i++)
		if (mod_lens[i] == opt_mod_length) {
			size = sizes[i];
			break;
		}
	if (size == -1) {
		fprintf(stderr, "Invalid modulus length.\n");
		return 1;
	}

	if (verbose)
		printf("Creating key files for %d keys.\n", opt_key_count);

	file = sc_file_new();
	if (!file) {
		fprintf(stderr, "out of memory.\n");
		return 1;
	}
	file->type = SC_FILE_TYPE_WORKING_EF;
	file->ef_structure = SC_FILE_EF_TRANSPARENT;

	file->id = 0x0012;
	file->size = opt_key_count * size + 3;
	sc_file_add_acl_entry(file, SC_AC_OP_READ, SC_AC_NEVER, SC_AC_KEY_REF_NONE);
	sc_file_add_acl_entry(file, SC_AC_OP_UPDATE, SC_AC_CHV, 1);
	sc_file_add_acl_entry(file, SC_AC_OP_INVALIDATE, SC_AC_CHV, 1);
	sc_file_add_acl_entry(file, SC_AC_OP_REHABILITATE, SC_AC_CHV, 1);

	if (select_app_df()) {
		sc_file_free(file);
		return 1;
	}
	r = sc_create_file(card, file);
	sc_file_free(file);
	if (r) {
		fprintf(stderr, "Unable to create private key file: %s\n", sc_strerror(r));
		return 1;
	}

	file = sc_file_new();
	if (!file) {
		fprintf(stderr, "out of memory.\n");
		return 1;
	}
	file->type = SC_FILE_TYPE_WORKING_EF;
	file->ef_structure = SC_FILE_EF_TRANSPARENT;

	file->id = 0x1012;
	file->size = opt_key_count * (size + 4) + 3;
	sc_file_add_acl_entry(file, SC_AC_OP_READ, SC_AC_NONE, SC_AC_KEY_REF_NONE);
	sc_file_add_acl_entry(file, SC_AC_OP_UPDATE, SC_AC_CHV, 1);
	sc_file_add_acl_entry(file, SC_AC_OP_INVALIDATE, SC_AC_CHV, 1);
	sc_file_add_acl_entry(file, SC_AC_OP_REHABILITATE, SC_AC_CHV, 1);

	if (select_app_df()) {
		sc_file_free(file);
		return 1;
	}
	r = sc_create_file(card, file);
	sc_file_free(file);
	if (r) {
		fprintf(stderr, "Unable to create public key file: %s\n", sc_strerror(r));
		return 1;
	}
	if (verbose)
		printf("Key files generated successfully.\n");
	return 0;
}