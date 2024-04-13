S_ssc_init(pTHX_ const RExC_state_t *pRExC_state, regnode_ssc *ssc)
{
    /* Initializes the SSC 'ssc'.  This includes setting it to match an empty
     * string, any code point, or any posix class under locale */

    PERL_ARGS_ASSERT_SSC_INIT;

    Zero(ssc, 1, regnode_ssc);
    set_ANYOF_SYNTHETIC(ssc);
    ARG_SET(ssc, ANYOF_ONLY_HAS_BITMAP);
    ssc_anything(ssc);

    /* If any portion of the regex is to operate under locale rules that aren't
     * fully known at compile time, initialization includes it.  The reason
     * this isn't done for all regexes is that the optimizer was written under
     * the assumption that locale was all-or-nothing.  Given the complexity and
     * lack of documentation in the optimizer, and that there are inadequate
     * test cases for locale, many parts of it may not work properly, it is
     * safest to avoid locale unless necessary. */
    if (RExC_contains_locale) {
	ANYOF_POSIXL_SETALL(ssc);
    }
    else {
	ANYOF_POSIXL_ZERO(ssc);
    }
}