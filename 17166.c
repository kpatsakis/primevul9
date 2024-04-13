nr2hex(unsigned c)
{
    if ((c & 0xf) <= 9)
	return (c & 0xf) + '0';
    return (c & 0xf) - 10 + 'a';
}