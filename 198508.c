static int tcos_construct_fci(const sc_file_t *file,
                              u8 *out, size_t *outlen)
{
	u8 *p = out;
	u8 buf[64];
        size_t n;

        /* FIXME: possible buffer overflow */

        *p++ = 0x6F; /* FCI */
        p++;

	/* File size */
	buf[0] = (file->size >> 8) & 0xFF;
	buf[1] = file->size & 0xFF;
	sc_asn1_put_tag(0x81, buf, 2, p, 16, &p);

        /* File descriptor */
        n = 0;
	buf[n] = file->shareable ? 0x40 : 0;
	switch (file->type) {
	case SC_FILE_TYPE_WORKING_EF:
		break;
	case SC_FILE_TYPE_DF:
		buf[0] |= 0x38;
		break;
	default:
		return SC_ERROR_NOT_SUPPORTED;
	}
	buf[n++] |= file->ef_structure & 7;
        if ( (file->ef_structure & 7) > 1) {
                /* record structured file */
                buf[n++] = 0x41; /* indicate 3rd byte */
                buf[n++] = file->record_length;
        }
	sc_asn1_put_tag(0x82, buf, n, p, 8, &p);

        /* File identifier */
	buf[0] = (file->id >> 8) & 0xFF;
	buf[1] = file->id & 0xFF;
	sc_asn1_put_tag(0x83, buf, 2, p, 16, &p);

        /* Directory name */
        if (file->type == SC_FILE_TYPE_DF) {
                if (file->namelen) {
                        sc_asn1_put_tag(0x84, file->name, file->namelen,
                                        p, 16, &p);
                }
                else {
                        /* TCOS needs one, so we use a faked one */
                        snprintf ((char *) buf, sizeof(buf)-1, "foo-%lu",
                                  (unsigned long) time (NULL));
                        sc_asn1_put_tag(0x84, buf, strlen ((char *) buf), p, 16, &p);
                }
        }

        /* File descriptor extension */
        if (file->prop_attr_len && file->prop_attr) {
		n = file->prop_attr_len;
		memcpy(buf, file->prop_attr, n);
        }
        else {
                n = 0;
                buf[n++] = 0x01; /* not invalidated, permanent */
                if (file->type == SC_FILE_TYPE_WORKING_EF) 
                        buf[n++] = 0x00; /* generic data file */
        }
        sc_asn1_put_tag(0x85, buf, n, p, 16, &p);

        /* Security attributes */
	if (file->sec_attr_len && file->sec_attr) {
		memcpy(buf, file->sec_attr, file->sec_attr_len);
		n = file->sec_attr_len;
	}
        else {
                /* no attributes given - fall back to default one */
                memcpy (buf+ 0, "\xa4\x00\x00\x00\xff\xff", 6); /* select */
                memcpy (buf+ 6, "\xb0\x00\x00\x00\xff\xff", 6); /* read bin */
                memcpy (buf+12, "\xd6\x00\x00\x00\xff\xff", 6); /* upd bin */
                memcpy (buf+18, "\x60\x00\x00\x00\xff\xff", 6); /* admin grp*/
                n = 24;
        }
        sc_asn1_put_tag(0x86, buf, n, p, sizeof (buf), &p);

        
        /* fixup length of FCI */
        out[1] = p - out - 2;

	*outlen = p - out;
	return 0;
}