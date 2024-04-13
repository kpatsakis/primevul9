MBSTRING_API char *php_unicode_convert_case(
		int case_mode, const char *srcstr, size_t srclen, size_t *ret_len,
		const mbfl_encoding *src_encoding, int illegal_mode, int illegal_substchar)
{
	struct convert_case_data data;
	mbfl_convert_filter *from_wchar, *to_wchar;
	mbfl_string result, *result_ptr;

	mbfl_memory_device device;
	mbfl_memory_device_init(&device, srclen + 1, 0);

	/* encoding -> wchar filter */
	to_wchar = mbfl_convert_filter_new(src_encoding,
			&mbfl_encoding_wchar, convert_case_filter, NULL, &data);
	if (to_wchar == NULL) {
		mbfl_memory_device_clear(&device);
		return NULL;
	}

	/* wchar -> encoding filter */
	from_wchar = mbfl_convert_filter_new(
			&mbfl_encoding_wchar, src_encoding,
			mbfl_memory_device_output, NULL, &device);
	if (from_wchar == NULL) {
		mbfl_convert_filter_delete(to_wchar);
		mbfl_memory_device_clear(&device);
		return NULL;
	}

	to_wchar->illegal_mode = illegal_mode;
	to_wchar->illegal_substchar = illegal_substchar;
	from_wchar->illegal_mode = illegal_mode;
	from_wchar->illegal_substchar = illegal_substchar;

	data.next_filter = from_wchar;
	data.no_encoding = src_encoding->no_encoding;
	data.case_mode = case_mode;
	data.title_mode = 0;

	{
		/* feed data */
		const unsigned char *p = (const unsigned char *) srcstr;
		size_t n = srclen;
		while (n > 0) {
			if ((*to_wchar->filter_function)(*p++, to_wchar) < 0) {
				break;
			}
			n--;
		}
	}

	mbfl_convert_filter_flush(to_wchar);
	mbfl_convert_filter_flush(from_wchar);
	result_ptr = mbfl_memory_device_result(&device, &result);
	mbfl_convert_filter_delete(to_wchar);
	mbfl_convert_filter_delete(from_wchar);

	if (!result_ptr) {
		return NULL;
	}

	*ret_len = result.len;
	return (char *) result.val;
}