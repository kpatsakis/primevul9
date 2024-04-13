static int coolkey_initialize(sc_card_t *card)
{
	int r;
	coolkey_private_data_t *priv = NULL;
	coolkey_life_cycle_t life_cycle;
	coolkey_object_info_t object_info;
	int combined_processed = 0;

	/* already found? */
	if (card->drv_data) {
		return SC_SUCCESS;
	}
	sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"Coolkey Applet found");

	priv = coolkey_new_private_data();
	if (priv == NULL) {
		r = SC_ERROR_OUT_OF_MEMORY;
		goto cleanup;
	}
	r = coolkey_get_life_cycle(card, &life_cycle);
	if (r < 0) {
		goto cleanup;
	}

	/* Select a coolkey read the coolkey objects out */
	r = coolkey_select_applet(card);
	if (r < 0) {
		goto cleanup;
	}

	priv->protocol_version_major = life_cycle.protocol_version_major;
	priv->protocol_version_minor = life_cycle.protocol_version_minor;
	priv->pin_count = life_cycle.pin_count;
	priv->life_cycle = life_cycle.life_cycle;

	/* walk down the list of objects and read them off the token */
	r = coolkey_list_object(card, COOLKEY_LIST_RESET, &object_info);
	while (r >= 0) {
		unsigned long object_id;
		unsigned short object_len;

		/* The card did not return what we expected: Lets try other objects */
		if ((size_t)r < (sizeof(object_info)))
			break;

		/* TODO also look at the ACL... */

		object_id = bebytes2ulong(object_info.object_id);
		object_len = bebytes2ulong(object_info.object_length);


		/* the combined object is a single object that can store the other objects.
		 * most coolkeys provisioned by TPS has a single combined object that is
		 * compressed greatly increasing the effectiveness of compress (since lots
		 * of certs on the token share the same Subject and Issuer DN's). We now
		 * process it separately so that we can have both combined objects managed
		 * by TPS and user managed certs on the same token */
		if (object_id == COOLKEY_COMBINED_OBJECT_ID) {
			u8 *object = malloc(object_len);
			if (object == NULL) {
				r = SC_ERROR_OUT_OF_MEMORY;
				break;
			}
			r = coolkey_read_object(card, COOLKEY_COMBINED_OBJECT_ID, 0, object, object_len,
											priv->nonce, sizeof(priv->nonce));
			if (r < 0) {
				free(object);
				break;
			}
			r = coolkey_process_combined_object(card, priv, object, r);
			free(object);
			if (r != SC_SUCCESS) {
				break;
			}
			combined_processed = 1;
		} else {
			r = coolkey_add_object(priv, object_id, NULL, object_len, 0);
			if (r != SC_SUCCESS)
				sc_log(card->ctx, "coolkey_add_object() returned %d", r);
		}

		/* Read next object: error is handled on the cycle condition and below after cycle */
		r = coolkey_list_object(card, COOLKEY_LIST_NEXT, &object_info);
	}
	if (r != SC_ERROR_FILE_END_REACHED) {
		/* This means the card does not cooperate at all: bail out */
		if (r >= 0) {
			r = SC_ERROR_INVALID_CARD;
		}
		goto cleanup;
	}
	/* if we didn't pull the cuid from the combined object, then grab it now */
	if (!combined_processed) {
		global_platform_cplc_data_t cplc_data;
		/* select the card manager, because a card with applet only will have
		   already selected the coolkey applet */

		r = gp_select_card_manager(card);
		if (r < 0) {
			goto cleanup;
		}

		r = gp_get_cplc_data(card, &cplc_data);
		if (r < 0) {
			goto cleanup;
		}
		coolkey_make_cuid_from_cplc(&priv->cuid, &cplc_data);
		priv->token_name = (u8 *)strdup("COOLKEY");
		if (priv->token_name == NULL) {
			r= SC_ERROR_OUT_OF_MEMORY;
			goto cleanup;
		}
		priv->token_name_length = sizeof("COOLKEY")-1;
	}
	card->drv_data = priv;
	return SC_SUCCESS;

cleanup:
	if (priv) {
		coolkey_free_private_data(priv);
	}
	return r;
}