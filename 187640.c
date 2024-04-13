struct accept *parse_accept(const char **hdr)
{
    int i, n = 0, alloc = 0;
    struct accept *ret = NULL;
#define GROW_ACCEPT 10;

    for (i = 0; hdr && hdr[i]; i++) {
        tok_t tok = TOK_INITIALIZER(hdr[i], ";,", TOK_TRIMLEFT|TOK_TRIMRIGHT);
        char *token;

        while ((token = tok_next(&tok))) {
            if (!strncmp(token, "q=", 2)) {
                if (!ret) break;
                ret[n-1].qual = strtof(token+2, NULL);
            }
            else {
                if (n + 1 >= alloc)  {
                    alloc += GROW_ACCEPT;
                    ret = xrealloc(ret, alloc * sizeof(struct accept));
                }
                ret[n].token = xstrdup(token);
                ret[n].qual = 1.0;
                ret[++n].token = NULL;
            }
        }
        tok_fini(&tok);
    }

    qsort(ret, n, sizeof(struct accept),
          (int (*)(const void *, const void *)) &compare_accept);

    return ret;
}