static unsigned etag_match(const char *hdr[], const char *etag)
{
    unsigned i, match = 0;
    tok_t tok;
    char *token;

    for (i = 0; !match && hdr[i]; i++) {
        tok_init(&tok, hdr[i], ",", TOK_TRIMLEFT|TOK_TRIMRIGHT);
        while (!match && (token = tok_next(&tok))) {
            if (!etagcmp(token, etag)) match = 1;
        }
        tok_fini(&tok);
    }

    return match;
}