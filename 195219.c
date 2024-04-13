PHP_FUNCTION(iptcparse)
{
	int inx = 0, len;
	unsigned int tagsfound = 0;
	unsigned char *buffer, recnum, dataset, key[ 16 ];
	char *str;
	int str_len;
	zval *values, **element;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) != SUCCESS) {
		return;
	}

	buffer = (unsigned char *)str;

	while (inx < str_len) { /* find 1st tag */
		if ((buffer[inx] == 0x1c) && ((buffer[inx+1] == 0x01) || (buffer[inx+1] == 0x02))){ 
			break;
		} else {
			inx++;
		}
	}

	while (inx < str_len) {
		if (buffer[ inx++ ] != 0x1c) {
			break;   /* we ran against some data which does not conform to IPTC - stop parsing! */
		} 
		
		if ((inx + 4) >= str_len)
			break;

		dataset = buffer[ inx++ ];
		recnum = buffer[ inx++ ];

		if (buffer[ inx ] & (unsigned char) 0x80) { /* long tag */
			if((inx+6) >= str_len) {
				break;
			}
			len = (((long) buffer[ inx + 2 ]) << 24) + (((long) buffer[ inx + 3 ]) << 16) + 
				  (((long) buffer[ inx + 4 ]) <<  8) + (((long) buffer[ inx + 5 ]));
			inx += 6;
		} else { /* short tag */
			len = (((unsigned short) buffer[ inx ])<<8) | (unsigned short)buffer[ inx+1 ];
			inx += 2;
		}
		
		if ((len < 0) || (len > str_len) || (inx + len) > str_len) {
			break;
		}

		snprintf(key, sizeof(key), "%d#%03d", (unsigned int) dataset, (unsigned int) recnum);

		if (tagsfound == 0) { /* found the 1st tag - initialize the return array */
			array_init(return_value);
		}

		if (zend_hash_find(Z_ARRVAL_P(return_value), key, strlen(key) + 1, (void **) &element) == FAILURE) {
			MAKE_STD_ZVAL(values);
			array_init(values);
			
			zend_hash_update(Z_ARRVAL_P(return_value), key, strlen(key) + 1, (void *) &values, sizeof(zval*), (void **) &element);
		} 
			
		add_next_index_stringl(*element, buffer+inx, len, 1);
		inx += len;
		tagsfound++;
	}

	if (! tagsfound) {
		RETURN_FALSE;
	}
}