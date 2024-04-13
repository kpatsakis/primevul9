static void line_header_fini(RzBinDwarfLineHeader *hdr) {
	if (hdr) {
		for (size_t i = 0; i < hdr->file_names_count; i++) {
			free(hdr->file_names[i].name);
		}

		free(hdr->std_opcode_lengths);
		free(hdr->file_names);

		if (hdr->include_dirs) {
			for (size_t i = 0; i < hdr->include_dirs_count; i++) {
				free(hdr->include_dirs[i]);
			}
			free(hdr->include_dirs);
		}
	}
}