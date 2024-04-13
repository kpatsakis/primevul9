PHPAPI char *_php_math_number_format_ex(double d, int dec, char *dec_point,
		size_t dec_point_len, char *thousand_sep, size_t thousand_sep_len)
{
	return _php_math_number_format_ex_len(d, dec, dec_point, dec_point_len,
			thousand_sep, thousand_sep_len, NULL);
}