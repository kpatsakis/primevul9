i_apply_case_fold(OnigCodePoint from, OnigCodePoint to[],
		  int to_len, void* arg)
{
  IApplyCaseFoldArg* iarg;
  ScanEnv* env;
  CClassNode* cc;
  BitSetRef bs;

  iarg = (IApplyCaseFoldArg* )arg;
  env = iarg->env;
  cc  = iarg->cc;
  bs = cc->bs;

  if (to_len == 1) {
    int is_in = onig_is_code_in_cc(env->enc, from, cc);
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
    if ((is_in != 0 && !IS_NCCLASS_NOT(cc)) ||
        (is_in == 0 &&  IS_NCCLASS_NOT(cc))) {
      if (ONIGENC_MBC_MINLEN(env->enc) > 1 || *to >= SINGLE_BYTE_SIZE) {
        add_code_range(&(cc->mbuf), env, *to, *to);
      }
      else {
        BITSET_SET_BIT(bs, *to);
      }
    }
#else
    if (is_in != 0) {
      if (ONIGENC_MBC_MINLEN(env->enc) > 1 || *to >= SINGLE_BYTE_SIZE) {
        if (IS_NCCLASS_NOT(cc)) clear_not_flag_cclass(cc, env->enc);
        add_code_range(&(cc->mbuf), env, *to, *to);
      }
      else {
        if (IS_NCCLASS_NOT(cc)) {
          BITSET_CLEAR_BIT(bs, *to);
        }
        else
          BITSET_SET_BIT(bs, *to);
      }
    }
#endif /* CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS */
  }
  else {
    int r, i, len;
    UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
    Node *snode = NULL_NODE;

    if (onig_is_code_in_cc(env->enc, from, cc)
#ifdef CASE_FOLD_IS_APPLIED_INSIDE_NEGATIVE_CCLASS
	&& !IS_NCCLASS_NOT(cc)
#endif
        ) {
      for (i = 0; i < to_len; i++) {
        len = ONIGENC_CODE_TO_MBC(env->enc, to[i], buf);
        if (i == 0) {
          snode = onig_node_new_str(buf, buf + len);
          CHECK_NULL_RETURN_MEMERR(snode);

          /* char-class expanded multi-char only
             compare with string folded at match time. */
          NSTRING_SET_AMBIG(snode);
        }
        else {
          r = onig_node_str_cat(snode, buf, buf + len);
          if (r < 0) {
            onig_node_free(snode);
            return r;
          }
        }
      }

      *(iarg->ptail) = onig_node_new_alt(snode, NULL_NODE);
      CHECK_NULL_RETURN_MEMERR(*(iarg->ptail));
      iarg->ptail = &(NODE_CDR((*(iarg->ptail))));
    }
  }

  return 0;
}