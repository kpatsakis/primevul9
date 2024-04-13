show_one_termcode(char_u *name, char_u *code, int printit)
{
    char_u	*p;
    int		len;

    if (name[0] > '~')
    {
	IObuff[0] = ' ';
	IObuff[1] = ' ';
	IObuff[2] = ' ';
	IObuff[3] = ' ';
    }
    else
    {
	IObuff[0] = 't';
	IObuff[1] = '_';
	IObuff[2] = name[0];
	IObuff[3] = name[1];
    }
    IObuff[4] = ' ';

    p = get_special_key_name(TERMCAP2KEY(name[0], name[1]), 0);
    if (p[1] != 't')
	STRCPY(IObuff + 5, p);
    else
	IObuff[5] = NUL;
    len = (int)STRLEN(IObuff);
    do
	IObuff[len++] = ' ';
    while (len < 17);
    IObuff[len] = NUL;
    if (code == NULL)
	len += 4;
    else
	len += vim_strsize(code);

    if (printit)
    {
	msg_puts((char *)IObuff);
	if (code == NULL)
	    msg_puts("NULL");
	else
	    msg_outtrans(code);
    }
    return len;
}