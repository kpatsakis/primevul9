illegal_char(char_u *errbuf, int c)
{
    if (errbuf == NULL)
	return (char_u *)"";
    sprintf((char *)errbuf, _("E539: Illegal character <%s>"),
							(char *)transchar(c));
    return errbuf;
}