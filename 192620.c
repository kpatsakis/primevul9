S_cntrl_to_mnemonic(const U8 c)
{
    /* Returns the mnemonic string that represents character 'c', if one
     * exists; NULL otherwise.  The only ones that exist for the purposes of
     * this routine are a few control characters */

    switch (c) {
        case '\a':       return "\\a";
        case '\b':       return "\\b";
        case ESC_NATIVE: return "\\e";
        case '\f':       return "\\f";
        case '\n':       return "\\n";
        case '\r':       return "\\r";
        case '\t':       return "\\t";
    }

    return NULL;
}