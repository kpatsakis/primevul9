next_state_val(CClassNode* cc, CClassNode* asc_cc,
	       OnigCodePoint *from, OnigCodePoint to,
	       int* from_israw, int to_israw,
	       enum CCVALTYPE intype, enum CCVALTYPE* type,
	       enum CCSTATE* state, ScanEnv* env)
{
  int r;

  switch (*state) {
  case CCS_VALUE:
    if (*type == CCV_SB) {
      BITSET_SET_BIT_CHKDUP(cc->bs, (int )(*from));
      if (IS_NOT_NULL(asc_cc))
	BITSET_SET_BIT(asc_cc->bs, (int )(*from));
    }
    else if (*type == CCV_CODE_POINT) {
      r = add_code_range(&(cc->mbuf), env, *from, *from);
      if (r < 0) return r;
      if (IS_NOT_NULL(asc_cc)) {
	r = add_code_range0(&(asc_cc->mbuf), env, *from, *from, 0);
	if (r < 0) return r;
      }
    }
    break;

  case CCS_RANGE:
    if (intype == *type) {
      if (intype == CCV_SB) {
	if (*from > 0xff || to > 0xff)
	  return ONIGERR_INVALID_CODE_POINT_VALUE;

	if (*from > to) {
	  if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	    goto ccs_range_end;
	  else
	    return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
	}
	bitset_set_range(env, cc->bs, (int )*from, (int )to);
	if (IS_NOT_NULL(asc_cc))
	  bitset_set_range(env, asc_cc->bs, (int )*from, (int )to);
      }
      else {
	r = add_code_range(&(cc->mbuf), env, *from, to);
	if (r < 0) return r;
	if (IS_NOT_NULL(asc_cc)) {
	  r = add_code_range0(&(asc_cc->mbuf), env, *from, to, 0);
	  if (r < 0) return r;
	}
      }
    }
    else {
      if (*from > to) {
	if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_EMPTY_RANGE_IN_CC))
	  goto ccs_range_end;
	else
	  return ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS;
      }
      bitset_set_range(env, cc->bs, (int )*from, (int )(to < 0xff ? to : 0xff));
      r = add_code_range(&(cc->mbuf), env, (OnigCodePoint )*from, to);
      if (r < 0) return r;
      if (IS_NOT_NULL(asc_cc)) {
	bitset_set_range(env, asc_cc->bs, (int )*from, (int )(to < 0xff ? to : 0xff));
	r = add_code_range0(&(asc_cc->mbuf), env, (OnigCodePoint )*from, to, 0);
	if (r < 0) return r;
      }
    }
  ccs_range_end:
    *state = CCS_COMPLETE;
    break;

  case CCS_COMPLETE:
  case CCS_START:
    *state = CCS_VALUE;
    break;

  default:
    break;
  }

  *from_israw = to_israw;
  *from       = to;
  *type       = intype;
  return 0;
}