static int parse_ranges(const char *hdr, unsigned long len,
                        struct range **ranges)
{
    int ret = HTTP_BAD_RANGE;
    struct range *new, *tail = *ranges = NULL;
    tok_t tok;
    char *token;

    if (!len) return HTTP_OK;  /* need to know length of representation */

    /* we only handle byte-unit */
    if (!hdr || strncmp(hdr, "bytes=", 6)) return HTTP_OK;

    tok_init(&tok, hdr+6, ",", TOK_TRIMLEFT|TOK_TRIMRIGHT);
    while ((token = tok_next(&tok))) {
        /* default to entire representation */
        unsigned long first = 0;
        unsigned long last = len - 1;
        char *p, *endp;

        if (!(p = strchr(token, '-'))) continue;  /* bad byte-range-set */

        if (p == token) {
            /* suffix-byte-range-spec */
            unsigned long suffix = strtoul(++p, &endp, 10);

            if (endp == p || *endp) continue;  /* bad suffix-length */
            if (!suffix) continue;      /* unsatisfiable suffix-length */

            /* don't start before byte zero */
            if (suffix < len) first = len - suffix;
        }
        else {
            /* byte-range-spec */
            first = strtoul(token, &endp, 10);
            if (endp != p) continue;      /* bad first-byte-pos */
            if (first >= len) continue;   /* unsatisfiable first-byte-pos */

            if (*++p) {
                /* last-byte-pos */
                last = strtoul(p, &endp, 10);
                if (*endp || last < first) continue; /* bad last-byte-pos */

                /* don't go past end of representation */
                if (last >= len) last = len - 1;
            }
        }

        ret = HTTP_PARTIAL;

        /* Coalesce overlapping ranges, or those with a gap < 80 bytes */
        if (tail &&
            first >= tail->first && (long) (first - tail->last) < 80) {
            tail->last = MAX(last, tail->last);
            continue;
        }

        /* Create a new range and append it to linked list */
        new = xzmalloc(sizeof(struct range));
        new->first = first;
        new->last = last;

        if (tail) tail->next = new;
        else *ranges = new;
        tail = new;
    }

    tok_fini(&tok);

    return ret;
}