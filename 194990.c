sc_oberthur_parse_containers (struct sc_pkcs15_card *p15card,
		unsigned char *buff, size_t len, int postpone_allowed)
{
	struct sc_context *ctx = p15card->card->ctx;
	size_t offs;

	LOG_FUNC_CALLED(ctx);

	while (Containers)   {
		struct container *next = Containers->next;

		free (Containers);
		Containers = next;
	}

	for (offs=0; offs < len;)  {
		struct container *cont;
		unsigned char *ptr =  buff + offs + 2;

		sc_log(ctx,
		       "parse contaniers offs:%"SC_FORMAT_LEN_SIZE_T"u, len:%"SC_FORMAT_LEN_SIZE_T"u",
		       offs, len);
		if (*(buff + offs) != 'R')
			return SC_ERROR_INVALID_DATA;

		cont = (struct container *)calloc(sizeof(struct container), 1);
		if (!cont)
			return SC_ERROR_OUT_OF_MEMORY;

		cont->exchange.id_pub = *ptr * 0x100 + *(ptr + 1);  ptr += 2;
		cont->exchange.id_prv = *ptr * 0x100 + *(ptr + 1);  ptr += 2;
		cont->exchange.id_cert = *ptr * 0x100 + *(ptr + 1); ptr += 2;

		cont->sign.id_pub = *ptr * 0x100 + *(ptr + 1);  ptr += 2;
		cont->sign.id_prv = *ptr * 0x100 + *(ptr + 1);  ptr += 2;
		cont->sign.id_cert = *ptr * 0x100 + *(ptr + 1); ptr += 2;

		memcpy(cont->uuid, ptr + 2, 36);
		sc_log(ctx, "UUID: %s; 0x%X, 0x%X, 0x%X", cont->uuid,
				cont->exchange.id_pub, cont->exchange.id_prv, cont->exchange.id_cert);

		if (!Containers)  {
			Containers = cont;
		}
		else   {
			cont->next = Containers;
			Containers->prev = (void *)cont;
			Containers = cont;
		}

		offs += *(buff + offs + 1) + 2;
	}

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}