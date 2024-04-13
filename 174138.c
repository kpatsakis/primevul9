LWZReadByte_(gdIOCtx *fd, LZW_STATIC_DATA *sd, char flag, int input_code_size, int *ZeroDataBlockP)
{
	int code, incode, i;

	if(flag) {
		sd->set_code_size = input_code_size;
		sd->code_size = sd->set_code_size + 1;
		sd->clear_code = 1 << sd->set_code_size;
		sd->end_code = sd->clear_code + 1;
		sd->max_code_size = 2 * sd->clear_code;
		sd->max_code = sd->clear_code + 2;

		GetCode(fd, &sd->scd, 0, TRUE, ZeroDataBlockP);

		sd->fresh = TRUE;

		for(i = 0; i < sd->clear_code; ++i) {
			sd->table[0][i] = 0;
			sd->table[1][i] = i;
		}

		for(; i < (1 << MAX_LWZ_BITS); ++i) {
			sd->table[0][i] = sd->table[1][0] = 0;
		}

		sd->sp = sd->stack;

		return 0;

	} else if(sd->fresh) {
		sd->fresh = FALSE;

		do {
			sd->firstcode = sd->oldcode =
			                    GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP);
		} while(sd->firstcode == sd->clear_code);

		return sd->firstcode;
	}

	if(sd->sp > sd->stack) {
		return *--sd->sp;
	}

	while((code = GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP)) >= 0) {
		if(code == sd->clear_code) {
			for(i = 0; i < sd->clear_code; ++i) {
				sd->table[0][i] = 0;
				sd->table[1][i] = i;
			}

			for (; i < (1 << MAX_LWZ_BITS); ++i) {
				sd->table[0][i] = sd->table[1][i] = 0;
			}

			sd->code_size = sd->set_code_size + 1;
			sd->max_code_size = 2 * sd->clear_code;
			sd->max_code = sd->clear_code + 2;
			sd->sp = sd->stack;
			sd->firstcode = sd->oldcode =
			                    GetCode(fd, &sd->scd, sd->code_size, FALSE, ZeroDataBlockP);

			return sd->firstcode;
		} else if(code == sd->end_code) {
			int count;
			unsigned char buf[260];

			if(*ZeroDataBlockP) {
				return -2;
			}

			while((count = GetDataBlock(fd, buf, ZeroDataBlockP)) > 0);

			if(count != 0) {
				return -2;
			}
		}

		incode = code;

		if(sd->sp == (sd->stack + STACK_SIZE)) {
			/* Bad compressed data stream */
			return -1;
		}

		if(code >= sd->max_code) {
			*sd->sp++ = sd->firstcode;
			code = sd->oldcode;
		}

		while(code >= sd->clear_code) {
			if(sd->sp == (sd->stack + STACK_SIZE)) {
				/* Bad compressed data stream */
				return -1;
			}
			if(code >= (1 << MAX_LWZ_BITS)) {
				/* Corrupted code */
				return -1;
			}

			*sd->sp++ = sd->table[1][code];

			if(code == sd->table[0][code]) {
				/* Oh well */
			}

			code = sd->table[0][code];
		}
		if(code >= (1 << MAX_LWZ_BITS)) {
			/* Corrupted code */
			return -1;
		}

		*sd->sp++ = sd->firstcode = sd->table[1][code];

		if((code = sd->max_code) < (1 << MAX_LWZ_BITS)) {
			sd->table[0][code] = sd->oldcode;
			sd->table[1][code] = sd->firstcode;
			++sd->max_code;

			if((sd->max_code >= sd->max_code_size) && (sd->max_code_size < (1<<MAX_LWZ_BITS))) {
				sd->max_code_size *= 2;
				++sd->code_size;
			}
		}

		sd->oldcode = incode;

		if(sd->sp > sd->stack) {
			return *--sd->sp;
		}
	}

	return code;
}