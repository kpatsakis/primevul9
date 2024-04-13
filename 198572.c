static int initialize(int reader_id, int verbose,
		sc_context_t **ctx, sc_reader_t **reader)
{
	unsigned int i, reader_count;
	int r;

	if (!ctx || !reader)
		return SC_ERROR_INVALID_ARGUMENTS;

	r = sc_establish_context(ctx, "");
	if (r < 0 || !*ctx) {
		fprintf(stderr, "Failed to create initial context: %s", sc_strerror(r));
		return r;
	}

	(*ctx)->debug = verbose;
	(*ctx)->flags |= SC_CTX_FLAG_ENABLE_DEFAULT_DRIVER;

	reader_count = sc_ctx_get_reader_count(*ctx);

	if (reader_count == 0) {
		sc_debug(*ctx, SC_LOG_DEBUG_NORMAL, "No reader not found.\n");
		return SC_ERROR_NO_READERS_FOUND;
	}

	if (reader_id < 0) {
		/* Automatically try to skip to a reader with a card if reader not specified */
		for (i = 0; i < reader_count; i++) {
			*reader = sc_ctx_get_reader(*ctx, i);
			if (sc_detect_card_presence(*reader) & SC_READER_CARD_PRESENT) {
				reader_id = i;
				sc_debug(*ctx, SC_LOG_DEBUG_NORMAL, "Using the first reader"
						" with a card: %s", (*reader)->name);
				break;
			}
		}
		if ((unsigned int) reader_id >= reader_count) {
			sc_debug(*ctx, SC_LOG_DEBUG_NORMAL, "No card found, using the first reader.");
			reader_id = 0;
		}
	}

	if ((unsigned int) reader_id >= reader_count) {
		sc_debug(*ctx, SC_LOG_DEBUG_NORMAL, "Invalid reader number "
				"(%d), only %d available.\n", reader_id, reader_count);
		return SC_ERROR_NO_READERS_FOUND;
	}

	*reader = sc_ctx_get_reader(*ctx, reader_id);

	return SC_SUCCESS;
}