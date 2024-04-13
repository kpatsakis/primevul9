static int coolkey_read_binary(sc_card_t *card, unsigned int idx,
		u8 *buf, size_t count, unsigned long flags)
{
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	int r = 0, len;
	u8 *data = NULL;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	if (idx > priv->obj->length) {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_FILE_END_REACHED);
	}

	/* if we've already read the data, just return it */
	if (priv->obj->data) {
		sc_log(card->ctx, 
			 "returning cached value idx=%u count=%"SC_FORMAT_LEN_SIZE_T"u",
			 idx, count);
		len = MIN(count, priv->obj->length-idx);
		memcpy(buf, &priv->obj->data[idx], len);
		LOG_FUNC_RETURN(card->ctx, len);
	}

	sc_log(card->ctx, 
		 "clearing cache idx=%u count=%"SC_FORMAT_LEN_SIZE_T"u",
		 idx, count);

	data = malloc(priv->obj->length);
	if (data == NULL) {
		r = SC_ERROR_OUT_OF_MEMORY;
		goto done;
	}


	r = coolkey_read_object(card, priv->obj->id, 0, data, priv->obj->length,
												priv->nonce, sizeof(priv->nonce));
	if (r < 0)
		goto done;

	if ((size_t) r != priv->obj->length) {
		priv->obj->length = r;
	}


	/* OK we've read the data, now copy the required portion out to the callers buffer */
	len = MIN(count, priv->obj->length-idx);
	memcpy(buf, &data[idx], len);
	r = len;
	/* cache the data in the object */
	priv->obj->data=data;
	data = NULL;

done:
	if (data)
		free(data);
	LOG_FUNC_RETURN(card->ctx, r);
}