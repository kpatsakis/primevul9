quote_x509_string( char* instr) {
	char * result_string = 0;
	int    result_string_len = 0;

	char * x509_fqan_escape = 0;
	char * x509_fqan_escape_sub = 0;
	char * x509_fqan_delimiter = 0;
	char * x509_fqan_delimiter_sub = 0;

	int x509_fqan_escape_sub_len = 0;
	int x509_fqan_delimiter_sub_len = 0;

	char * tmp_scan_ptr;

	// NULL in, NULL out
	if (!instr) {
		return NULL;
	}

	// we look at first char only.  default '&'.
    if (!(x509_fqan_escape = param("X509_FQAN_ESCAPE"))) {
		x509_fqan_escape = strdup("&");
	}
	// can be multiple chars
    if (!(x509_fqan_escape_sub = param("X509_FQAN_ESCAPE_SUB"))) {
		x509_fqan_escape_sub = strdup("&amp;");
	}

	// we look at first char only.  default ','.
    if (!(x509_fqan_delimiter = param("X509_FQAN_DELIMITER"))) {
		x509_fqan_delimiter = strdup(",");
	}
	// can be multiple chars
    if (!(x509_fqan_delimiter_sub = param("X509_FQAN_DELIMITER_SUB"))) {
		x509_fqan_delimiter_sub = strdup("&comma;");
	}


	// phase 0, trim quotes off if needed
	// use tmp_scan_ptr to temporarily hold trimmed strings while being reassigned.
	tmp_scan_ptr = trim_quotes(x509_fqan_escape);
	free (x509_fqan_escape);
	x509_fqan_escape = tmp_scan_ptr;

	tmp_scan_ptr = trim_quotes(x509_fqan_escape_sub);
	free (x509_fqan_escape_sub);
	x509_fqan_escape_sub = tmp_scan_ptr;
	x509_fqan_escape_sub_len = strlen(x509_fqan_escape_sub);

	tmp_scan_ptr = trim_quotes(x509_fqan_delimiter);
	free (x509_fqan_delimiter);
	x509_fqan_delimiter = tmp_scan_ptr;

	tmp_scan_ptr = trim_quotes(x509_fqan_delimiter_sub);
	free (x509_fqan_delimiter_sub);
	x509_fqan_delimiter_sub = tmp_scan_ptr;
	x509_fqan_delimiter_sub_len = strlen(x509_fqan_delimiter_sub);


	// phase 1, scan the string to compute the new length
	result_string_len = 0;
	for (tmp_scan_ptr = instr; *tmp_scan_ptr; tmp_scan_ptr++) {
		if( (*tmp_scan_ptr)==x509_fqan_escape[0] ) {
			result_string_len += x509_fqan_escape_sub_len;
		} else if( (*tmp_scan_ptr)==x509_fqan_delimiter[0] ) {
			result_string_len += x509_fqan_delimiter_sub_len;
		} else {
			result_string_len++;
		}
	}

	// phase 2, process the string into the result buffer

	// malloc new string (with NULL terminator)
	result_string = (char*) malloc (result_string_len + 1);
	ASSERT( result_string );
	*result_string = 0;
	result_string_len = 0;

	for (tmp_scan_ptr = instr; *tmp_scan_ptr; tmp_scan_ptr++) {
		if( (*tmp_scan_ptr)==x509_fqan_escape[0] ) {
			strcat(&(result_string[result_string_len]), x509_fqan_escape_sub);
			result_string_len += x509_fqan_escape_sub_len;
		} else if( (*tmp_scan_ptr)==x509_fqan_delimiter[0] ) {
			strcat(&(result_string[result_string_len]), x509_fqan_delimiter_sub);
			result_string_len += x509_fqan_delimiter_sub_len;
		} else {
			result_string[result_string_len] = *tmp_scan_ptr;
			result_string_len++;
		}
		result_string[result_string_len] = 0;
	}

	// clean up
	free(x509_fqan_escape);
	free(x509_fqan_escape_sub);
	free(x509_fqan_delimiter);
	free(x509_fqan_delimiter_sub);

	return result_string;
}