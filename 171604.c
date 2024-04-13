ldns_pkt_tsig_sign_next(ldns_pkt *pkt, const char *key_name, const char *key_data,
	uint16_t fudge, const char *algorithm_name, const ldns_rdf *query_mac, int tsig_timers_only)
{
	ldns_rr *tsig_rr;
	ldns_rdf *key_name_rdf = ldns_rdf_new_frm_str(LDNS_RDF_TYPE_DNAME, key_name);
	ldns_rdf *fudge_rdf = NULL;
	ldns_rdf *orig_id_rdf = NULL;
	ldns_rdf *algorithm_rdf;
	ldns_rdf *error_rdf = NULL;
	ldns_rdf *mac_rdf = NULL;
	ldns_rdf *other_data_rdf = NULL;

	ldns_status status = LDNS_STATUS_OK;

	uint8_t *pkt_wire = NULL;
	size_t pkt_wire_len;

	struct timeval tv_time_signed;
	uint8_t *time_signed = NULL;
	ldns_rdf *time_signed_rdf = NULL;

	algorithm_rdf = ldns_rdf_new_frm_str(LDNS_RDF_TYPE_DNAME, algorithm_name);
	if(!key_name_rdf || !algorithm_rdf) {
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}

	/* eww don't have create tsigtime rdf yet :( */
	/* bleh :p */
	if (gettimeofday(&tv_time_signed, NULL) == 0) {
		time_signed = LDNS_XMALLOC(uint8_t, 6);
		if(!time_signed) {
			status = LDNS_STATUS_MEM_ERR;
			goto clean;
		}
		ldns_write_uint64_as_uint48(time_signed,
				(uint64_t)tv_time_signed.tv_sec);
	} else {
		status = LDNS_STATUS_INTERNAL_ERR;
		goto clean;
	}

	time_signed_rdf = ldns_rdf_new(LDNS_RDF_TYPE_TSIGTIME, 6, time_signed);
	if(!time_signed_rdf) {
		LDNS_FREE(time_signed);
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}

	fudge_rdf = ldns_native2rdf_int16(LDNS_RDF_TYPE_INT16, fudge);

	orig_id_rdf = ldns_native2rdf_int16(LDNS_RDF_TYPE_INT16, ldns_pkt_id(pkt));

	error_rdf = ldns_native2rdf_int16(LDNS_RDF_TYPE_INT16, 0);

	other_data_rdf = ldns_native2rdf_int16_data(0, NULL);

	if(!fudge_rdf || !orig_id_rdf || !error_rdf || !other_data_rdf) {
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}

	if (ldns_pkt2wire(&pkt_wire, pkt, &pkt_wire_len) != LDNS_STATUS_OK) {
		status = LDNS_STATUS_ERR;
		goto clean;
	}

	status = ldns_tsig_mac_new(&mac_rdf, pkt_wire, pkt_wire_len,
			key_data, key_name_rdf, fudge_rdf, algorithm_rdf,
			time_signed_rdf, error_rdf, other_data_rdf, query_mac, tsig_timers_only);

	if (!mac_rdf) {
		goto clean;
	}

	LDNS_FREE(pkt_wire);

	/* Create the TSIG RR */
	tsig_rr = ldns_rr_new();
	if(!tsig_rr) {
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}
	ldns_rr_set_owner(tsig_rr, key_name_rdf);
	ldns_rr_set_class(tsig_rr, LDNS_RR_CLASS_ANY);
	ldns_rr_set_type(tsig_rr, LDNS_RR_TYPE_TSIG);
	ldns_rr_set_ttl(tsig_rr, 0);

	ldns_rr_push_rdf(tsig_rr, algorithm_rdf);
	ldns_rr_push_rdf(tsig_rr, time_signed_rdf);
	ldns_rr_push_rdf(tsig_rr, fudge_rdf);
	ldns_rr_push_rdf(tsig_rr, mac_rdf);
	ldns_rr_push_rdf(tsig_rr, orig_id_rdf);
	ldns_rr_push_rdf(tsig_rr, error_rdf);
	ldns_rr_push_rdf(tsig_rr, other_data_rdf);

	ldns_pkt_set_tsig(pkt, tsig_rr);

	return status;

  clean:
	LDNS_FREE(pkt_wire);
	ldns_rdf_free(key_name_rdf);
	ldns_rdf_free(algorithm_rdf);
	ldns_rdf_free(time_signed_rdf);
	ldns_rdf_free(fudge_rdf);
	ldns_rdf_free(orig_id_rdf);
	ldns_rdf_free(error_rdf);
	ldns_rdf_free(other_data_rdf);
	return status;
}