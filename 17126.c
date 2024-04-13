transchar_byte(int c)
{
    if (enc_utf8 && c >= 0x80)
    {
	transchar_nonprint(curbuf, transchar_charbuf, c);
	return transchar_charbuf;
    }
    return transchar(c);
}