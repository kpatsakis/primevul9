toggle_Magic(int x)
{
    if (is_Magic(x))
	return un_Magic(x);
    return Magic(x);
}