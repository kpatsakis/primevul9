static bcon_token_t bcon_token(char *s) {
    if (s == 0) return Token_EOD;
    switch (s[0]) {
    case ':': if (s[1] != '\0' && s[2] != '\0' && s[3] != '\0' && s[4] == '\0' &&
                      s[3] == ':' && (s[1] == '_' || s[1] == 'P' || s[1] == 'R'))
            return Token_Typespec; break;
    case '{': if (s[1] == '\0') return Token_OpenBrace; break;
    case '}': if (s[1] == '\0') return Token_CloseBrace; break;
    case '[': if (s[1] == '\0') return Token_OpenBracket; break;
    case ']': if (s[1] == '\0') return Token_CloseBracket; break;
    case '.': if (s[1] == '\0') return Token_End; break;
    }
    return Token_Default;
}