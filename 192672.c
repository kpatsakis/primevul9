S_parse_lparen_question_flags(pTHX_ RExC_state_t *pRExC_state)
{
    /* This parses the flags that are in either the '(?foo)' or '(?foo:bar)'
     * constructs, and updates RExC_flags with them.  On input, RExC_parse
     * should point to the first flag; it is updated on output to point to the
     * final ')' or ':'.  There needs to be at least one flag, or this will
     * abort */

    /* for (?g), (?gc), and (?o) warnings; warning
       about (?c) will warn about (?g) -- japhy    */

#define WASTED_O  0x01
#define WASTED_G  0x02
#define WASTED_C  0x04
#define WASTED_GC (WASTED_G|WASTED_C)
    I32 wastedflags = 0x00;
    U32 posflags = 0, negflags = 0;
    U32 *flagsp = &posflags;
    char has_charset_modifier = '\0';
    regex_charset cs;
    bool has_use_defaults = FALSE;
    const char* const seqstart = RExC_parse - 1; /* Point to the '?' */
    int x_mod_count = 0;

    PERL_ARGS_ASSERT_PARSE_LPAREN_QUESTION_FLAGS;

    /* '^' as an initial flag sets certain defaults */
    if (UCHARAT(RExC_parse) == '^') {
        RExC_parse++;
        has_use_defaults = TRUE;
        STD_PMMOD_FLAGS_CLEAR(&RExC_flags);
        set_regex_charset(&RExC_flags, (RExC_utf8 || RExC_uni_semantics)
                                        ? REGEX_UNICODE_CHARSET
                                        : REGEX_DEPENDS_CHARSET);
    }

    cs = get_regex_charset(RExC_flags);
    if (cs == REGEX_DEPENDS_CHARSET
        && (RExC_utf8 || RExC_uni_semantics))
    {
        cs = REGEX_UNICODE_CHARSET;
    }

    while (RExC_parse < RExC_end) {
        /* && strchr("iogcmsx", *RExC_parse) */
        /* (?g), (?gc) and (?o) are useless here
           and must be globally applied -- japhy */
        switch (*RExC_parse) {

            /* Code for the imsxn flags */
            CASE_STD_PMMOD_FLAGS_PARSE_SET(flagsp, x_mod_count);

            case LOCALE_PAT_MOD:
                if (has_charset_modifier) {
                    goto excess_modifier;
                }
                else if (flagsp == &negflags) {
                    goto neg_modifier;
                }
                cs = REGEX_LOCALE_CHARSET;
                has_charset_modifier = LOCALE_PAT_MOD;
                break;
            case UNICODE_PAT_MOD:
                if (has_charset_modifier) {
                    goto excess_modifier;
                }
                else if (flagsp == &negflags) {
                    goto neg_modifier;
                }
                cs = REGEX_UNICODE_CHARSET;
                has_charset_modifier = UNICODE_PAT_MOD;
                break;
            case ASCII_RESTRICT_PAT_MOD:
                if (flagsp == &negflags) {
                    goto neg_modifier;
                }
                if (has_charset_modifier) {
                    if (cs != REGEX_ASCII_RESTRICTED_CHARSET) {
                        goto excess_modifier;
                    }
                    /* Doubled modifier implies more restricted */
                    cs = REGEX_ASCII_MORE_RESTRICTED_CHARSET;
                }
                else {
                    cs = REGEX_ASCII_RESTRICTED_CHARSET;
                }
                has_charset_modifier = ASCII_RESTRICT_PAT_MOD;
                break;
            case DEPENDS_PAT_MOD:
                if (has_use_defaults) {
                    goto fail_modifiers;
                }
                else if (flagsp == &negflags) {
                    goto neg_modifier;
                }
                else if (has_charset_modifier) {
                    goto excess_modifier;
                }

                /* The dual charset means unicode semantics if the
                 * pattern (or target, not known until runtime) are
                 * utf8, or something in the pattern indicates unicode
                 * semantics */
                cs = (RExC_utf8 || RExC_uni_semantics)
                     ? REGEX_UNICODE_CHARSET
                     : REGEX_DEPENDS_CHARSET;
                has_charset_modifier = DEPENDS_PAT_MOD;
                break;
              excess_modifier:
                RExC_parse++;
                if (has_charset_modifier == ASCII_RESTRICT_PAT_MOD) {
                    vFAIL2("Regexp modifier \"%c\" may appear a maximum of twice", ASCII_RESTRICT_PAT_MOD);
                }
                else if (has_charset_modifier == *(RExC_parse - 1)) {
                    vFAIL2("Regexp modifier \"%c\" may not appear twice",
                                        *(RExC_parse - 1));
                }
                else {
                    vFAIL3("Regexp modifiers \"%c\" and \"%c\" are mutually exclusive", has_charset_modifier, *(RExC_parse - 1));
                }
                NOT_REACHED; /*NOTREACHED*/
              neg_modifier:
                RExC_parse++;
                vFAIL2("Regexp modifier \"%c\" may not appear after the \"-\"",
                                    *(RExC_parse - 1));
                NOT_REACHED; /*NOTREACHED*/
            case ONCE_PAT_MOD: /* 'o' */
            case GLOBAL_PAT_MOD: /* 'g' */
                if (PASS2 && ckWARN(WARN_REGEXP)) {
                    const I32 wflagbit = *RExC_parse == 'o'
                                         ? WASTED_O
                                         : WASTED_G;
                    if (! (wastedflags & wflagbit) ) {
                        wastedflags |= wflagbit;
			/* diag_listed_as: Useless (?-%s) - don't use /%s modifier in regex; marked by <-- HERE in m/%s/ */
                        vWARN5(
                            RExC_parse + 1,
                            "Useless (%s%c) - %suse /%c modifier",
                            flagsp == &negflags ? "?-" : "?",
                            *RExC_parse,
                            flagsp == &negflags ? "don't " : "",
                            *RExC_parse
                        );
                    }
                }
                break;

            case CONTINUE_PAT_MOD: /* 'c' */
                if (PASS2 && ckWARN(WARN_REGEXP)) {
                    if (! (wastedflags & WASTED_C) ) {
                        wastedflags |= WASTED_GC;
			/* diag_listed_as: Useless (?-%s) - don't use /%s modifier in regex; marked by <-- HERE in m/%s/ */
                        vWARN3(
                            RExC_parse + 1,
                            "Useless (%sc) - %suse /gc modifier",
                            flagsp == &negflags ? "?-" : "?",
                            flagsp == &negflags ? "don't " : ""
                        );
                    }
                }
                break;
            case KEEPCOPY_PAT_MOD: /* 'p' */
                if (flagsp == &negflags) {
                    if (PASS2)
                        ckWARNreg(RExC_parse + 1,"Useless use of (?-p)");
                } else {
                    *flagsp |= RXf_PMf_KEEPCOPY;
                }
                break;
            case '-':
                /* A flag is a default iff it is following a minus, so
                 * if there is a minus, it means will be trying to
                 * re-specify a default which is an error */
                if (has_use_defaults || flagsp == &negflags) {
                    goto fail_modifiers;
                }
                flagsp = &negflags;
                wastedflags = 0;  /* reset so (?g-c) warns twice */
                x_mod_count = 0;
                break;
            case ':':
            case ')':

                if ((posflags & (RXf_PMf_EXTENDED|RXf_PMf_EXTENDED_MORE)) == RXf_PMf_EXTENDED) {
                    negflags |= RXf_PMf_EXTENDED_MORE;
                }
                RExC_flags |= posflags;

                if (negflags & RXf_PMf_EXTENDED) {
                    negflags |= RXf_PMf_EXTENDED_MORE;
                }
                RExC_flags &= ~negflags;
                set_regex_charset(&RExC_flags, cs);

                return;
            default:
              fail_modifiers:
                RExC_parse += SKIP_IF_CHAR(RExC_parse);
		/* diag_listed_as: Sequence (?%s...) not recognized in regex; marked by <-- HERE in m/%s/ */
                vFAIL2utf8f("Sequence (%" UTF8f "...) not recognized",
                      UTF8fARG(UTF, RExC_parse-seqstart, seqstart));
                NOT_REACHED; /*NOTREACHED*/
        }

        RExC_parse += UTF ? UTF8SKIP(RExC_parse) : 1;
    }

    vFAIL("Sequence (?... not terminated");
}