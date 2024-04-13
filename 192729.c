PHPAPI char *_php_math_number_format(double d, int dec, char dec_point, char thousand_sep)
{
	return _php_math_number_format_ex(d, dec, &dec_point, 1, &thousand_sep, 1);
}