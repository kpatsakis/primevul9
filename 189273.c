reset_option_was_set(char_u *name)
{
    int idx = findoption(name);

    if (idx >= 0)
	options[idx].flags &= ~P_WAS_SET;
}