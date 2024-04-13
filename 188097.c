parse_char_class(Node** np, Node** asc_np, OnigToken* tok, UChar** src, UChar* end,
		 ScanEnv* env)
{
  int r, neg, len, fetched, and_start;
  OnigCodePoint v, vs;
  UChar *p;
  Node* node;
  Node* asc_node;
  CClassNode *cc, *prev_cc;
  CClassNode *asc_cc, *asc_prev_cc;
  CClassNode work_cc, asc_work_cc;

  enum CCSTATE state;
  enum CCVALTYPE val_type, in_type;
  int val_israw, in_israw;

  *np = *asc_np = NULL_NODE;
  env->parse_depth++;
  if (env->parse_depth > ParseDepthLimit)
    return ONIGERR_PARSE_DEPTH_LIMIT_OVER;
  prev_cc = asc_prev_cc = (CClassNode* )NULL;
  r = fetch_token_in_cc(tok, src, end, env);
  if (r == TK_CHAR && tok->u.c == '^' && tok->escaped == 0) {
    neg = 1;
    r = fetch_token_in_cc(tok, src, end, env);
  }
  else {
    neg = 0;
  }

  if (r < 0) return r;
  if (r == TK_CC_CLOSE) {
    if (! code_exist_check((OnigCodePoint )']',
                           *src, env->pattern_end, 1, env))
      return ONIGERR_EMPTY_CHAR_CLASS;

    CC_ESC_WARN(env, (UChar* )"]");
    r = tok->type = TK_CHAR;  /* allow []...] */
  }

  *np = node = node_new_cclass();
  CHECK_NULL_RETURN_MEMERR(node);
  cc = NCCLASS(node);

  if (IS_IGNORECASE(env->option)) {
    *asc_np = asc_node = node_new_cclass();
    CHECK_NULL_RETURN_MEMERR(asc_node);
    asc_cc = NCCLASS(asc_node);
  }
  else {
    asc_node = NULL_NODE;
    asc_cc = NULL;
  }

  and_start = 0;
  state = CCS_START;
  p = *src;
  while (r != TK_CC_CLOSE) {
    fetched = 0;
    switch (r) {
    case TK_CHAR:
      if ((tok->u.code >= SINGLE_BYTE_SIZE) ||
	  (len = ONIGENC_CODE_TO_MBCLEN(env->enc, tok->u.c)) > 1) {
	in_type = CCV_CODE_POINT;
      }
      else if (len < 0) {
	r = len;
	goto err;
      }
      else {
      sb_char:
	in_type = CCV_SB;
      }
      v = (OnigCodePoint )tok->u.c;
      in_israw = 0;
      goto val_entry2;
      break;

    case TK_RAW_BYTE:
      /* tok->base != 0 : octal or hexadec. */
      if (! ONIGENC_IS_SINGLEBYTE(env->enc) && tok->base != 0) {
	UChar buf[ONIGENC_CODE_TO_MBC_MAXLEN];
	UChar* bufe = buf + ONIGENC_CODE_TO_MBC_MAXLEN;
	UChar* psave = p;
	int i, base = tok->base;

	buf[0] = (UChar )tok->u.c;
	for (i = 1; i < ONIGENC_MBC_MAXLEN(env->enc); i++) {
	  r = fetch_token_in_cc(tok, &p, end, env);
	  if (r < 0) goto err;
	  if (r != TK_RAW_BYTE || tok->base != base) {
	    fetched = 1;
	    break;
	  }
	  buf[i] = (UChar )tok->u.c;
	}

	if (i < ONIGENC_MBC_MINLEN(env->enc)) {
	  r = ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
	  goto err;
	}

	len = enclen(env->enc, buf, buf + i);
	if (i < len) {
	  r = ONIGERR_TOO_SHORT_MULTI_BYTE_STRING;
	  goto err;
	}
	else if (i > len) { /* fetch back */
	  p = psave;
	  for (i = 1; i < len; i++) {
	    (void)fetch_token_in_cc(tok, &p, end, env);
	    /* no need to check the retun value (already checked above) */
	  }
	  fetched = 0;
	}

	if (i == 1) {
	  v = (OnigCodePoint )buf[0];
	  goto raw_single;
	}
	else {
	  v = ONIGENC_MBC_TO_CODE(env->enc, buf, bufe);
	  in_type = CCV_CODE_POINT;
	}
      }
      else {
	v = (OnigCodePoint )tok->u.c;
      raw_single:
	in_type = CCV_SB;
      }
      in_israw = 1;
      goto val_entry2;
      break;

    case TK_CODE_POINT:
      v = tok->u.code;
      in_israw = 1;
    val_entry:
      len = ONIGENC_CODE_TO_MBCLEN(env->enc, v);
      if (len < 0) {
	r = len;
	goto err;
      }
      in_type = (len == 1 ? CCV_SB : CCV_CODE_POINT);
    val_entry2:
      r = next_state_val(cc, asc_cc, &vs, v, &val_israw, in_israw, in_type, &val_type,
			 &state, env);
      if (r != 0) goto err;
      break;

    case TK_POSIX_BRACKET_OPEN:
      r = parse_posix_bracket(cc, asc_cc, &p, end, env);
      if (r < 0) goto err;
      if (r == 1) {  /* is not POSIX bracket */
	CC_ESC_WARN(env, (UChar* )"[");
	p = tok->backp;
	v = (OnigCodePoint )tok->u.c;
	in_israw = 0;
	goto val_entry;
      }
      goto next_class;
      break;

    case TK_CHAR_TYPE:
      r = add_ctype_to_cc(cc, tok->u.prop.ctype, tok->u.prop.not,
			  IS_ASCII_RANGE(env->option), env);
      if (r != 0) return r;
      if (IS_NOT_NULL(asc_cc)) {
	if (tok->u.prop.ctype != ONIGENC_CTYPE_WORD)
	  r = add_ctype_to_cc(asc_cc, tok->u.prop.ctype, tok->u.prop.not,
			      IS_ASCII_RANGE(env->option), env);
	if (r != 0) return r;
      }

    next_class:
      r = next_state_class(cc, asc_cc, &vs, &val_type, &state, env);
      if (r != 0) goto err;
      break;

    case TK_CHAR_PROPERTY:
      {
	int ctype;

	ctype = fetch_char_property_to_ctype(&p, end, env);
	if (ctype < 0) return ctype;
	r = add_ctype_to_cc(cc, ctype, tok->u.prop.not, 0, env);
	if (r != 0) return r;
	if (IS_NOT_NULL(asc_cc)) {
	  if (ctype != ONIGENC_CTYPE_ASCII)
	    r = add_ctype_to_cc(asc_cc, ctype, tok->u.prop.not, 0, env);
	  if (r != 0) return r;
	}
	goto next_class;
      }
      break;

    case TK_CC_RANGE:
      if (state == CCS_VALUE) {
	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	if (r == TK_CC_CLOSE) { /* allow [x-] */
	range_end_val:
	  v = (OnigCodePoint )'-';
	  in_israw = 0;
	  goto val_entry;
	}
	else if (r == TK_CC_AND) {
	  CC_ESC_WARN(env, (UChar* )"-");
	  goto range_end_val;
	}

	if (val_type == CCV_CLASS) {
	  r = ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS;
	  goto err;
	}

	state = CCS_RANGE;
      }
      else if (state == CCS_START) {
	/* [-xa] is allowed */
	v = (OnigCodePoint )tok->u.c;
	in_israw = 0;

	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	/* [--x] or [a&&-x] is warned. */
	if (r == TK_CC_RANGE || and_start != 0)
	  CC_ESC_WARN(env, (UChar* )"-");

	goto val_entry;
      }
      else if (state == CCS_RANGE) {
	CC_ESC_WARN(env, (UChar* )"-");
	goto sb_char;  /* [!--x] is allowed */
      }
      else { /* CCS_COMPLETE */
	r = fetch_token_in_cc(tok, &p, end, env);
	if (r < 0) goto err;
	fetched = 1;
	if (r == TK_CC_CLOSE) goto range_end_val; /* allow [a-b-] */
	else if (r == TK_CC_AND) {
	  CC_ESC_WARN(env, (UChar* )"-");
	  goto range_end_val;
	}

	if (IS_SYNTAX_BV(env->syntax, ONIG_SYN_ALLOW_DOUBLE_RANGE_OP_IN_CC)) {
	  CC_ESC_WARN(env, (UChar* )"-");
	  goto range_end_val;   /* [0-9-a] is allowed as [0-9\-a] */
	}
	r = ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS;
	goto err;
      }
      break;

    case TK_CC_CC_OPEN: /* [ */
      {
	Node *anode, *aasc_node;
	CClassNode* acc;

	r = parse_char_class(&anode, &aasc_node, tok, &p, end, env);
	if (r == 0) {
	  acc = NCCLASS(anode);
	  r = or_cclass(cc, acc, env);
	}
	if (r == 0 && IS_NOT_NULL(aasc_node)) {
	  acc = NCCLASS(aasc_node);
	  r = or_cclass(asc_cc, acc, env);
	}
	onig_node_free(anode);
	onig_node_free(aasc_node);
	if (r != 0) goto err;
      }
      break;

    case TK_CC_AND: /* && */
      {
	if (state == CCS_VALUE) {
	  r = next_state_val(cc, asc_cc, &vs, 0, &val_israw, 0, val_type,
			     &val_type, &state, env);
	  if (r != 0) goto err;
	}
	/* initialize local variables */
	and_start = 1;
	state = CCS_START;

	if (IS_NOT_NULL(prev_cc)) {
	  r = and_cclass(prev_cc, cc, env);
	  if (r != 0) goto err;
	  bbuf_free(cc->mbuf);
	  if (IS_NOT_NULL(asc_cc)) {
	    r = and_cclass(asc_prev_cc, asc_cc, env);
	    if (r != 0) goto err;
	    bbuf_free(asc_cc->mbuf);
	  }
	}
	else {
	  prev_cc = cc;
	  cc = &work_cc;
	  if (IS_NOT_NULL(asc_cc)) {
	    asc_prev_cc = asc_cc;
	    asc_cc = &asc_work_cc;
	  }
	}
	initialize_cclass(cc);
	if (IS_NOT_NULL(asc_cc))
	  initialize_cclass(asc_cc);
      }
      break;

    case TK_EOT:
      r = ONIGERR_PREMATURE_END_OF_CHAR_CLASS;
      goto err;
      break;
    default:
      r = ONIGERR_PARSER_BUG;
      goto err;
      break;
    }

    if (fetched)
      r = tok->type;
    else {
      r = fetch_token_in_cc(tok, &p, end, env);
      if (r < 0) goto err;
    }
  }

  if (state == CCS_VALUE) {
    r = next_state_val(cc, asc_cc, &vs, 0, &val_israw, 0, val_type,
		       &val_type, &state, env);
    if (r != 0) goto err;
  }

  if (IS_NOT_NULL(prev_cc)) {
    r = and_cclass(prev_cc, cc, env);
    if (r != 0) goto err;
    bbuf_free(cc->mbuf);
    cc = prev_cc;
    if (IS_NOT_NULL(asc_cc)) {
      r = and_cclass(asc_prev_cc, asc_cc, env);
      if (r != 0) goto err;
      bbuf_free(asc_cc->mbuf);
      asc_cc = asc_prev_cc;
    }
  }

  if (neg != 0) {
    NCCLASS_SET_NOT(cc);
    if (IS_NOT_NULL(asc_cc))
      NCCLASS_SET_NOT(asc_cc);
  }
  else {
    NCCLASS_CLEAR_NOT(cc);
    if (IS_NOT_NULL(asc_cc))
      NCCLASS_CLEAR_NOT(asc_cc);
  }
  if (IS_NCCLASS_NOT(cc) &&
      IS_SYNTAX_BV(env->syntax, ONIG_SYN_NOT_NEWLINE_IN_NEGATIVE_CC)) {
    int is_empty;

    is_empty = (IS_NULL(cc->mbuf) ? 1 : 0);
    if (is_empty != 0)
      BITSET_IS_EMPTY(cc->bs, is_empty);

    if (is_empty == 0) {
#define NEWLINE_CODE    0x0a

      if (ONIGENC_IS_CODE_NEWLINE(env->enc, NEWLINE_CODE)) {
	if (ONIGENC_CODE_TO_MBCLEN(env->enc, NEWLINE_CODE) == 1)
	  BITSET_SET_BIT_CHKDUP(cc->bs, NEWLINE_CODE);
	else {
	  r = add_code_range(&(cc->mbuf), env, NEWLINE_CODE, NEWLINE_CODE);
	  if (r < 0) goto err;
	}
      }
    }
  }
  *src = p;
  env->parse_depth--;
  return 0;

 err:
  if (cc != NCCLASS(*np))
    bbuf_free(cc->mbuf);
  if (IS_NOT_NULL(asc_cc) && (asc_cc != NCCLASS(*asc_np)))
    bbuf_free(asc_cc->mbuf);
  return r;
}