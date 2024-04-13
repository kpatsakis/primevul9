is_match_charset(
    FontData	font_data,
    char	*font_name)
{
    char *last;
    int length, name_len;

    name_len = strlen(font_name);
    last = font_name + name_len;

    length = strlen(font_data->name);
    if (length > name_len)
	return False;

    if (_XlcCompareISOLatin1(last - length, font_data->name) == 0)
	return True;

    return False;
}