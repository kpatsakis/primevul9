string_to_key(char_u *arg)
{
    if (*arg == '<')
	return find_key_option(arg + 1);
    if (*arg == '^')
	return Ctrl_chr(arg[1]);
    return *arg;
}