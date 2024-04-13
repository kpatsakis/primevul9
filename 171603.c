ldns_tsig_mac_new(ldns_rdf **tsig_mac, const uint8_t *pkt_wire, size_t pkt_wire_size,
		const char *key_data, const ldns_rdf *key_name_rdf, const ldns_rdf *fudge_rdf,
		const ldns_rdf *algorithm_rdf, const ldns_rdf *time_signed_rdf, const ldns_rdf *error_rdf,
		const ldns_rdf *other_data_rdf, const ldns_rdf *orig_mac_rdf, int tsig_timers_only)
{
	ldns_status status;
	char *wireformat;
	int wiresize;
	unsigned char *mac_bytes = NULL;
	unsigned char *key_bytes = NULL;
	int key_size;
	const EVP_MD *digester;
	char *algorithm_name = NULL;
	unsigned int md_len = EVP_MAX_MD_SIZE;
	ldns_rdf *result = NULL;
	ldns_buffer *data_buffer = NULL;
	ldns_rdf *canonical_key_name_rdf = NULL;
	ldns_rdf *canonical_algorithm_rdf = NULL;
	
	if (key_name_rdf == NULL || algorithm_rdf == NULL) {
		return LDNS_STATUS_NULL;
	}
	canonical_key_name_rdf  = ldns_rdf_clone(key_name_rdf);
	if (canonical_key_name_rdf == NULL) {
		return LDNS_STATUS_MEM_ERR;
	}
	canonical_algorithm_rdf = ldns_rdf_clone(algorithm_rdf);
	if (canonical_algorithm_rdf == NULL) {
		ldns_rdf_deep_free(canonical_key_name_rdf);
		return LDNS_STATUS_MEM_ERR;
	}
	/*
	 * prepare the digestable information
	 */
	data_buffer = ldns_buffer_new(LDNS_MAX_PACKETLEN);
	if (!data_buffer) {
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}
	/* if orig_mac is not NULL, add it too */
	if (orig_mac_rdf) {
		(void) ldns_rdf2buffer_wire(data_buffer, orig_mac_rdf);
 	}
	ldns_buffer_write(data_buffer, pkt_wire, pkt_wire_size);
	if (!tsig_timers_only) {
		ldns_dname2canonical(canonical_key_name_rdf);
		(void)ldns_rdf2buffer_wire(data_buffer, 
				canonical_key_name_rdf);
		ldns_buffer_write_u16(data_buffer, LDNS_RR_CLASS_ANY);
		ldns_buffer_write_u32(data_buffer, 0);
		ldns_dname2canonical(canonical_algorithm_rdf);
		(void)ldns_rdf2buffer_wire(data_buffer, 
				canonical_algorithm_rdf);
	}
	(void)ldns_rdf2buffer_wire(data_buffer, time_signed_rdf);
	(void)ldns_rdf2buffer_wire(data_buffer, fudge_rdf);
	if (!tsig_timers_only) {
		(void)ldns_rdf2buffer_wire(data_buffer, error_rdf);
		(void)ldns_rdf2buffer_wire(data_buffer, other_data_rdf);
	}

	wireformat = (char *) data_buffer->_data;
	wiresize = (int) ldns_buffer_position(data_buffer);

	algorithm_name = ldns_rdf2str(algorithm_rdf);
	if(!algorithm_name) {
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}

	/* prepare the key */
	key_bytes = LDNS_XMALLOC(unsigned char,
			ldns_b64_pton_calculate_size(strlen(key_data)));
	if(!key_bytes) {
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}
	key_size = ldns_b64_pton(key_data, key_bytes,
	ldns_b64_pton_calculate_size(strlen(key_data)));
	if (key_size < 0) {
		status = LDNS_STATUS_INVALID_B64;
		goto clean;
	}
	/* hmac it */
	/* 2 spare bytes for the length */
	mac_bytes = LDNS_XMALLOC(unsigned char, md_len+2);
	if(!mac_bytes) {
		status = LDNS_STATUS_MEM_ERR;
		goto clean;
	}
	memset(mac_bytes, 0, md_len+2);

	digester = ldns_digest_function(algorithm_name);

	if (digester) {
		(void) HMAC(digester, key_bytes, key_size, (void *)wireformat,
		            (size_t) wiresize, mac_bytes + 2, &md_len);

		ldns_write_uint16(mac_bytes, md_len);
		result = ldns_rdf_new_frm_data(LDNS_RDF_TYPE_INT16_DATA, md_len + 2,
				mac_bytes);
	} else {
		status = LDNS_STATUS_CRYPTO_UNKNOWN_ALGO;
		goto clean;
	}
	*tsig_mac = result;
	status = LDNS_STATUS_OK;
  clean:
	LDNS_FREE(mac_bytes);
	LDNS_FREE(key_bytes);
	LDNS_FREE(algorithm_name);
	ldns_buffer_free(data_buffer);
	ldns_rdf_deep_free(canonical_algorithm_rdf);
	ldns_rdf_deep_free(canonical_key_name_rdf);
	return status;
}