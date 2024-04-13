CC_DUP_WARN(ScanEnv *env, OnigCodePoint from ARG_UNUSED, OnigCodePoint to ARG_UNUSED)
{
  if (onig_warn == onig_null_warn || !RTEST(ruby_verbose)) return ;

  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_WARN_CC_DUP) &&
      !(env->warnings_flag & ONIG_SYN_WARN_CC_DUP)) {
#ifdef WARN_ALL_CC_DUP
    onig_syntax_warn(env, "character class has duplicated range: %04x-%04x", from, to);
#else
    env->warnings_flag |= ONIG_SYN_WARN_CC_DUP;
    onig_syntax_warn(env, "character class has duplicated range");
#endif
  }
}