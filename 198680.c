sc_pkcs15emu_get_df(sc_pkcs15_card_t *p15card, unsigned int type)
{
	sc_pkcs15_df_t	*df;
	sc_file_t	*file;
	int		created = 0;

	while (1) {
		for (df = p15card->df_list; df; df = df->next) {
			if (df->type == type) {
				if (created)
					df->enumerated = 1;
				return df;
			}
		}

		assert(created == 0);

		file = sc_file_new();
		if (!file)
			return NULL;
		sc_format_path("11001101", &file->path);
		sc_pkcs15_add_df(p15card, type, &file->path);
		sc_file_free(file);
		created++;
	}
}