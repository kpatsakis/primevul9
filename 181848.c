lyp_check_pattern(struct ly_ctx *ctx, const char *pattern, pcre **pcre_precomp)
{
    int idx, idx2, start, end, err_offset, count;
    char *perl_regex, *ptr;
    const char *err_msg, *orig_ptr;
    pcre *precomp;

    /*
     * adjust the expression to a Perl equivalent
     *
     * http://www.w3.org/TR/2004/REC-xmlschema-2-20041028/#regexs
     */

    /* we need to replace all "$" with "\$", count them now */
    for (count = 0, ptr = strchr(pattern, '$'); ptr; ++count, ptr = strchr(ptr + 1, '$'));

    perl_regex = malloc((strlen(pattern) + 4 + count) * sizeof(char));
    LY_CHECK_ERR_RETURN(!perl_regex, LOGMEM(ctx), EXIT_FAILURE);
    perl_regex[0] = '\0';

    ptr = perl_regex;

    if (strncmp(pattern + strlen(pattern) - 2, ".*", 2)) {
        /* we wil add line-end anchoring */
        ptr[0] = '(';
        ++ptr;
    }

    for (orig_ptr = pattern; orig_ptr[0]; ++orig_ptr) {
        if (orig_ptr[0] == '$') {
            ptr += sprintf(ptr, "\\$");
        } else {
            ptr[0] = orig_ptr[0];
            ++ptr;
        }
    }

    if (strncmp(pattern + strlen(pattern) - 2, ".*", 2)) {
        ptr += sprintf(ptr, ")$");
    } else {
        ptr[0] = '\0';
        ++ptr;
    }

    /* substitute Unicode Character Blocks with exact Character Ranges */
    while ((ptr = strstr(perl_regex, "\\p{Is"))) {
        start = ptr - perl_regex;

        ptr = strchr(ptr, '}');
        if (!ptr) {
            LOGVAL(ctx, LYE_INREGEX, LY_VLOG_NONE, NULL, pattern, perl_regex + start + 2, "unterminated character property");
            free(perl_regex);
            return EXIT_FAILURE;
        }

        end = (ptr - perl_regex) + 1;

        /* need more space */
        if (end - start < LYP_URANGE_LEN) {
            perl_regex = ly_realloc(perl_regex, strlen(perl_regex) + (LYP_URANGE_LEN - (end - start)) + 1);
            LY_CHECK_ERR_RETURN(!perl_regex, LOGMEM(ctx); free(perl_regex), EXIT_FAILURE);
        }

        /* find our range */
        for (idx = 0; lyp_ublock2urange[idx][0]; ++idx) {
            if (!strncmp(perl_regex + start + 5, lyp_ublock2urange[idx][0], strlen(lyp_ublock2urange[idx][0]))) {
                break;
            }
        }
        if (!lyp_ublock2urange[idx][0]) {
            LOGVAL(ctx, LYE_INREGEX, LY_VLOG_NONE, NULL, pattern, perl_regex + start + 5, "unknown block name");
            free(perl_regex);
            return EXIT_FAILURE;
        }

        /* make the space in the string and replace the block (but we cannot include brackets if it was already enclosed in them) */
        for (idx2 = 0, count = 0; idx2 < start; ++idx2) {
            if ((perl_regex[idx2] == '[') && (!idx2 || (perl_regex[idx2 - 1] != '\\'))) {
                ++count;
            }
            if ((perl_regex[idx2] == ']') && (!idx2 || (perl_regex[idx2 - 1] != '\\'))) {
                --count;
            }
        }
        if (count) {
            /* skip brackets */
            memmove(perl_regex + start + (LYP_URANGE_LEN - 2), perl_regex + end, strlen(perl_regex + end) + 1);
            memcpy(perl_regex + start, lyp_ublock2urange[idx][1] + 1, LYP_URANGE_LEN - 2);
        } else {
            memmove(perl_regex + start + LYP_URANGE_LEN, perl_regex + end, strlen(perl_regex + end) + 1);
            memcpy(perl_regex + start, lyp_ublock2urange[idx][1], LYP_URANGE_LEN);
        }
    }

    /* must return 0, already checked during parsing */
    precomp = pcre_compile(perl_regex, PCRE_ANCHORED | PCRE_DOLLAR_ENDONLY | PCRE_NO_AUTO_CAPTURE,
                           &err_msg, &err_offset, NULL);
    if (!precomp) {
        LOGVAL(ctx, LYE_INREGEX, LY_VLOG_NONE, NULL, pattern, perl_regex + err_offset, err_msg);
        free(perl_regex);
        return EXIT_FAILURE;
    }
    free(perl_regex);

    if (pcre_precomp) {
        *pcre_precomp = precomp;
    } else {
        free(precomp);
    }

    return EXIT_SUCCESS;
}