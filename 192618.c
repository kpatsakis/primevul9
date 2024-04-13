S_debug_show_study_flags(pTHX_ U32 flags, const char *open_str,
                                    const char *close_str)
{
    if (!flags)
        return;

    Perl_re_printf( aTHX_  "%s", open_str);
    DEBUG_SHOW_STUDY_FLAG(flags, SF_BEFORE_SEOL);
    DEBUG_SHOW_STUDY_FLAG(flags, SF_BEFORE_MEOL);
    DEBUG_SHOW_STUDY_FLAG(flags, SF_IS_INF);
    DEBUG_SHOW_STUDY_FLAG(flags, SF_HAS_PAR);
    DEBUG_SHOW_STUDY_FLAG(flags, SF_IN_PAR);
    DEBUG_SHOW_STUDY_FLAG(flags, SF_HAS_EVAL);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_DO_SUBSTR);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_DO_STCLASS_AND);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_DO_STCLASS_OR);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_DO_STCLASS);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_WHILEM_VISITED_POS);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_TRIE_RESTUDY);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_SEEN_ACCEPT);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_TRIE_DOING_RESTUDY);
    DEBUG_SHOW_STUDY_FLAG(flags, SCF_IN_DEFINE);
    Perl_re_printf( aTHX_  "%s", close_str);
}