char* trim_quotes( char* instr ) {
	char * result;
	int  instr_len;

	if (instr == NULL) {
		return NULL;
	}

	instr_len = strlen(instr);
	// to trim, must be minimum three characters with a double quote first and last
	if ((instr_len > 2) && (instr[0] == '"') && (instr[instr_len-1]) == '"') {
		// alloc a shorter buffer, copy everything in, and null terminate
		result = (char*)malloc(instr_len - 1); // minus two quotes, plus one NULL terminator.
		strncpy(result, &(instr[1]), instr_len-2);
		result[instr_len-2] = 0;
	} else {
		result = strdup(instr);
	}

	return result;
}