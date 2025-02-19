base64_to_byte_array (gunichar2 *start, gint ilength, MonoBoolean allowWhitespaceOnly)
{
	gint ignored;
	gint i;
	gunichar2 c;
	gunichar2 last, prev_last, prev2_last;
	gint olength;
	MonoArray *result;
	guchar *res_ptr;
	gint a [4], b [4];
	MonoException *exc;

	ignored = 0;
	last = prev_last = 0, prev2_last = 0;
	for (i = 0; i < ilength; i++) {
		c = start [i];
		if (c >= sizeof (dbase64)) {
			exc = mono_exception_from_name_msg (mono_get_corlib (),
				"System", "FormatException",
				"Invalid character found.");
			mono_raise_exception (exc);
		} else if (isspace (c)) {
			ignored++;
		} else {
			prev2_last = prev_last;
			prev_last = last;
			last = c;
		}
	}

	olength = ilength - ignored;

	if (allowWhitespaceOnly && olength == 0) {
		return mono_array_new (mono_domain_get (), mono_defaults.byte_class, 0);
	}

	if ((olength & 3) != 0 || olength <= 0) {
		exc = mono_exception_from_name_msg (mono_get_corlib (), "System",
					"FormatException", "Invalid length.");
		mono_raise_exception (exc);
	}

	if (prev2_last == '=') {
		exc = mono_exception_from_name_msg (mono_get_corlib (), "System", "FormatException", "Invalid format.");
		mono_raise_exception (exc);
	}

	olength = (olength * 3) / 4;
	if (last == '=')
		olength--;

	if (prev_last == '=')
		olength--;

	result = mono_array_new (mono_domain_get (), mono_defaults.byte_class, olength);
	res_ptr = mono_array_addr (result, guchar, 0);
	for (i = 0; i < ilength; ) {
		int k;

		for (k = 0; k < 4 && i < ilength;) {
			c = start [i++];
			if (isspace (c))
				continue;

			a [k] = (guchar) c;
			if (((b [k] = dbase64 [c]) & 0x80) != 0) {
				exc = mono_exception_from_name_msg (mono_get_corlib (),
					"System", "FormatException",
					"Invalid character found.");
				mono_raise_exception (exc);
			}
			k++;
		}

		*res_ptr++ = (b [0] << 2) | (b [1] >> 4);
		if (a [2] != '=')
			*res_ptr++ = (b [1] << 4) | (b [2] >> 2);
		if (a [3] != '=')
			*res_ptr++ = (b [2] << 6) | b [3];

		while (i < ilength && isspace (start [i]))
			i++;
	}

	return result;
}