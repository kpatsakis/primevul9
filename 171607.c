ldns_tsig_prepare_pkt_wire(const uint8_t *wire, size_t wire_len, size_t *result_len)
{
	uint8_t *wire2 = NULL;
	uint16_t qd_count;
	uint16_t an_count;
	uint16_t ns_count;
	uint16_t ar_count;
	ldns_rr *rr;

	size_t pos;
	uint16_t i;

	ldns_status status;

	if(wire_len < LDNS_HEADER_SIZE) {
		return NULL;
	}
	/* fake parse the wire */
	qd_count = LDNS_QDCOUNT(wire);
	an_count = LDNS_ANCOUNT(wire);
	ns_count = LDNS_NSCOUNT(wire);
	ar_count = LDNS_ARCOUNT(wire);

	if (ar_count > 0) {
		ar_count--;
	} else {
		return NULL;
	}

	pos = LDNS_HEADER_SIZE;

	for (i = 0; i < qd_count; i++) {
		status = ldns_wire2rr(&rr, wire, wire_len, &pos, LDNS_SECTION_QUESTION);
		if (status != LDNS_STATUS_OK) {
			return NULL;
		}
		ldns_rr_free(rr);
	}

	for (i = 0; i < an_count; i++) {
		status = ldns_wire2rr(&rr, wire, wire_len, &pos, LDNS_SECTION_ANSWER);
		if (status != LDNS_STATUS_OK) {
			return NULL;
		}
		ldns_rr_free(rr);
	}

	for (i = 0; i < ns_count; i++) {
		status = ldns_wire2rr(&rr, wire, wire_len, &pos, LDNS_SECTION_AUTHORITY);
		if (status != LDNS_STATUS_OK) {
			return NULL;
		}
		ldns_rr_free(rr);
	}

	for (i = 0; i < ar_count; i++) {
		status = ldns_wire2rr(&rr, wire, wire_len, &pos,
				LDNS_SECTION_ADDITIONAL);
		if (status != LDNS_STATUS_OK) {
			return NULL;
		}
		ldns_rr_free(rr);
	}

	*result_len = pos;
	wire2 = LDNS_XMALLOC(uint8_t, *result_len);
	if(!wire2) {
		return NULL;
	}
	memcpy(wire2, wire, *result_len);

	ldns_write_uint16(wire2 + LDNS_ARCOUNT_OFF, ar_count);

	return wire2;
}