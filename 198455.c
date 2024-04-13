util_connect_card_ex(sc_context_t *ctx, sc_card_t **cardp,
		 const char *reader_id, int do_wait, int do_lock, int verbose)
{
	struct sc_reader *reader = NULL, *found = NULL;
	struct sc_card *card = NULL;
	int r;

	sc_notify_init();

	if (do_wait) {
		unsigned int event;

		if (sc_ctx_get_reader_count(ctx) == 0) {
			fprintf(stderr, "Waiting for a reader to be attached...\n");
			r = sc_wait_for_event(ctx, SC_EVENT_READER_ATTACHED, &found, &event, -1, NULL);
			if (r < 0) {
				fprintf(stderr, "Error while waiting for a reader: %s\n", sc_strerror(r));
				return 3;
			}
			r = sc_ctx_detect_readers(ctx);
			if (r < 0) {
				fprintf(stderr, "Error while refreshing readers: %s\n", sc_strerror(r));
				return 3;
			}
		}
		fprintf(stderr, "Waiting for a card to be inserted...\n");
		r = sc_wait_for_event(ctx, SC_EVENT_CARD_INSERTED, &found, &event, -1, NULL);
		if (r < 0) {
			fprintf(stderr, "Error while waiting for a card: %s\n", sc_strerror(r));
			return 3;
		}
		reader = found;
	}
	else if (sc_ctx_get_reader_count(ctx) == 0) {
		fprintf(stderr, "No smart card readers found.\n");
		return 1;
	}
	else   {
		if (!reader_id) {
			unsigned int i;
			/* Automatically try to skip to a reader with a card if reader not specified */
			for (i = 0; i < sc_ctx_get_reader_count(ctx); i++) {
				reader = sc_ctx_get_reader(ctx, i);
				if (sc_detect_card_presence(reader) & SC_READER_CARD_PRESENT) {
					fprintf(stderr, "Using reader with a card: %s\n", reader->name);
					goto autofound;
				}
			}
			/* If no reader had a card, default to the first reader */
			reader = sc_ctx_get_reader(ctx, 0);
		}
		else {
			/* If the reader identifier looks like an ATR, try to find the reader with that card */
			if (is_string_valid_atr(reader_id))   {
				unsigned char atr_buf[SC_MAX_ATR_SIZE];
				size_t atr_buf_len = sizeof(atr_buf);
				unsigned int i;

				sc_hex_to_bin(reader_id, atr_buf, &atr_buf_len);
				/* Loop readers, looking for a card with ATR */
				for (i = 0; i < sc_ctx_get_reader_count(ctx); i++) {
					struct sc_reader *rdr = sc_ctx_get_reader(ctx, i);

					if (!(sc_detect_card_presence(rdr) & SC_READER_CARD_PRESENT))
						continue;
					else if (rdr->atr.len != atr_buf_len)
						continue;
					else if (memcmp(rdr->atr.value, atr_buf, rdr->atr.len))
						continue;

					fprintf(stderr, "Matched ATR in reader: %s\n", rdr->name);
					reader = rdr;
					goto autofound;
				}
			}
			else   {
				char *endptr = NULL;
				unsigned int num;

				errno = 0;
				num = strtol(reader_id, &endptr, 0);
				if (!errno && endptr && *endptr == '\0')
					reader = sc_ctx_get_reader(ctx, num);
				else
					reader = sc_ctx_get_reader_by_name(ctx, reader_id);
			}
		}
autofound:
		if (!reader) {
			fprintf(stderr, "Reader \"%s\" not found (%d reader(s) detected)\n",
					reader_id, sc_ctx_get_reader_count(ctx));
			return 1;
		}

		if (sc_detect_card_presence(reader) <= 0) {
			fprintf(stderr, "Card not present.\n");
			return 3;
		}
	}

	if (verbose)
		printf("Connecting to card in reader %s...\n", reader->name);
	r = sc_connect_card(reader, &card);
	if (r < 0) {
		fprintf(stderr, "Failed to connect to card: %s\n", sc_strerror(r));
		return 1;
	}

	if (verbose)
		printf("Using card driver %s.\n", card->driver->name);

	if (do_lock) {
		r = sc_lock(card);
		if (r < 0) {
			fprintf(stderr, "Failed to lock card: %s\n", sc_strerror(r));
			sc_disconnect_card(card);
			return 1;
		}
	}

	*cardp = card;
	return 0;
}