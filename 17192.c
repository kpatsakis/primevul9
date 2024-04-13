transchar_nonprint(buf_T *buf, char_u *charbuf, int c)
{
    if (c == NL)
	c = NUL;		// we use newline in place of a NUL
    else if (c == CAR && get_fileformat(buf) == EOL_MAC)
	c = NL;			// we use CR in place of  NL in this case

    if (dy_flags & DY_UHEX)		// 'display' has "uhex"
	transchar_hex(charbuf, c);

    else if (c <= 0x7f)			// 0x00 - 0x1f and 0x7f
    {
	charbuf[0] = '^';
	charbuf[1] = c ^ 0x40;		// DEL displayed as ^?
	charbuf[2] = NUL;
    }
    else if (enc_utf8 && c >= 0x80)
    {
	transchar_hex(charbuf, c);
    }
    else if (c >= ' ' + 0x80 && c <= '~' + 0x80)    // 0xa0 - 0xfe
    {
	charbuf[0] = '|';
	charbuf[1] = c - 0x80;
	charbuf[2] = NUL;
    }
    else					    // 0x80 - 0x9f and 0xff
    {
	charbuf[0] = '~';
	charbuf[1] = (c - 0x80) ^ 0x40;	// 0xff displayed as ~?
	charbuf[2] = NUL;
    }
}