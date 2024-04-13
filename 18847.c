decode_modifiers(int n)
{
    int	    code = n - 1;
    int	    modifiers = 0;

    if (code & 1)
	modifiers |= MOD_MASK_SHIFT;
    if (code & 2)
	modifiers |= MOD_MASK_ALT;
    if (code & 4)
	modifiers |= MOD_MASK_CTRL;
    if (code & 8)
	modifiers |= MOD_MASK_META;
    return modifiers;
}