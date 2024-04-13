dns_zone_keydone(dns_zone_t *zone, const char *keystr) {
	isc_result_t result = ISC_R_SUCCESS;
	isc_event_t *e;
	isc_buffer_t b;
	dns_zone_t *dummy = NULL;
	struct keydone *kd;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);

	e = isc_event_allocate(zone->mctx, zone, DNS_EVENT_KEYDONE, keydone,
			       zone, sizeof(struct keydone));
	if (e == NULL) {
		result = ISC_R_NOMEMORY;
		goto failure;
	}

	kd = (struct keydone *) e;
	if (strcasecmp(keystr, "all") == 0) {
		kd->all = true;
	} else {
		isc_textregion_t r;
		const char *algstr;
		dns_keytag_t keyid;
		dns_secalg_t alg;
		size_t n;

		kd->all = false;

		n = sscanf(keystr, "%hu/", &keyid);
		if (n == 0U) {
			CHECK(ISC_R_FAILURE);
		}

		algstr = strchr(keystr, '/');
		if (algstr != NULL) {
			algstr++;
		} else {
			CHECK(ISC_R_FAILURE);
		}

		n = sscanf(algstr, "%hhu", &alg);
		if (n == 0U) {
			DE_CONST(algstr, r.base);
			r.length = strlen(algstr);
			CHECK(dns_secalg_fromtext(&alg, &r));
		}

		/* construct a private-type rdata */
		isc_buffer_init(&b, kd->data, sizeof(kd->data));
		isc_buffer_putuint8(&b, alg);
		isc_buffer_putuint8(&b, (keyid & 0xff00) >> 8);
		isc_buffer_putuint8(&b, (keyid & 0xff));
		isc_buffer_putuint8(&b, 0);
		isc_buffer_putuint8(&b, 1);
	}

	zone_iattach(zone, &dummy);
	isc_task_send(zone->task, &e);

 failure:
	if (e != NULL) {
		isc_event_free(&e);
	}
	UNLOCK_ZONE(zone);
	return (result);
}