match_at(regex_t* reg, const UChar* str, const UChar* end,
         const UChar* in_right_range, const UChar* sstart, UChar* sprev,
         MatchArg* msa)
{

#if defined(USE_DIRECT_THREADED_CODE)
  static Operation FinishCode[] = { { .opaddr=&&L_FINISH, .opcode=OP_FINISH } };
#else
  static Operation FinishCode[] = { { OP_FINISH } };
#endif

#ifdef USE_GOTO_LABELS_AS_VALUES
  static const void *opcode_to_label[] = {
  &&L_FINISH,
  &&L_END,
  &&L_EXACT1,
  &&L_EXACT2,
  &&L_EXACT3,
  &&L_EXACT4,
  &&L_EXACT5,
  &&L_EXACTN,
  &&L_EXACTMB2N1,
  &&L_EXACTMB2N2,
  &&L_EXACTMB2N3,
  &&L_EXACTMB2N,
  &&L_EXACTMB3N,
  &&L_EXACTMBN,
  &&L_EXACT1_IC,
  &&L_EXACTN_IC,
  &&L_CCLASS,
  &&L_CCLASS_MB,
  &&L_CCLASS_MIX,
  &&L_CCLASS_NOT,
  &&L_CCLASS_MB_NOT,
  &&L_CCLASS_MIX_NOT,
  &&L_ANYCHAR,
  &&L_ANYCHAR_ML,
  &&L_ANYCHAR_STAR,
  &&L_ANYCHAR_ML_STAR,
  &&L_ANYCHAR_STAR_PEEK_NEXT,
  &&L_ANYCHAR_ML_STAR_PEEK_NEXT,
  &&L_WORD,
  &&L_WORD_ASCII,
  &&L_NO_WORD,
  &&L_NO_WORD_ASCII,
  &&L_WORD_BOUNDARY,
  &&L_NO_WORD_BOUNDARY,
  &&L_WORD_BEGIN,
  &&L_WORD_END,
  &&L_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY,
  &&L_NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY,
  &&L_BEGIN_BUF,
  &&L_END_BUF,
  &&L_BEGIN_LINE,
  &&L_END_LINE,
  &&L_SEMI_END_BUF,
  &&L_BEGIN_POSITION,
  &&L_BACKREF1,
  &&L_BACKREF2,
  &&L_BACKREF_N,
  &&L_BACKREF_N_IC,
  &&L_BACKREF_MULTI,
  &&L_BACKREF_MULTI_IC,
  &&L_BACKREF_WITH_LEVEL,
  &&L_BACKREF_WITH_LEVEL_IC,
  &&L_BACKREF_CHECK,
  &&L_BACKREF_CHECK_WITH_LEVEL,
  &&L_MEMORY_START,
  &&L_MEMORY_START_PUSH,
  &&L_MEMORY_END_PUSH,
  &&L_MEMORY_END_PUSH_REC,
  &&L_MEMORY_END,
  &&L_MEMORY_END_REC,
  &&L_FAIL,
  &&L_JUMP,
  &&L_PUSH,
  &&L_PUSH_SUPER,
  &&L_POP_OUT,
#ifdef USE_OP_PUSH_OR_JUMP_EXACT
  &&L_PUSH_OR_JUMP_EXACT1,
#endif
  &&L_PUSH_IF_PEEK_NEXT,
  &&L_REPEAT,
  &&L_REPEAT_NG,
  &&L_REPEAT_INC,
  &&L_REPEAT_INC_NG,
  &&L_REPEAT_INC_SG,
  &&L_REPEAT_INC_NG_SG,
  &&L_EMPTY_CHECK_START,
  &&L_EMPTY_CHECK_END,
  &&L_EMPTY_CHECK_END_MEMST,
  &&L_EMPTY_CHECK_END_MEMST_PUSH,
  &&L_PREC_READ_START,
  &&L_PREC_READ_END,
  &&L_PREC_READ_NOT_START,
  &&L_PREC_READ_NOT_END,
  &&L_ATOMIC_START,
  &&L_ATOMIC_END,
  &&L_LOOK_BEHIND,
  &&L_LOOK_BEHIND_NOT_START,
  &&L_LOOK_BEHIND_NOT_END,
  &&L_CALL,
  &&L_RETURN,
  &&L_PUSH_SAVE_VAL,
  &&L_UPDATE_VAR,
#ifdef USE_CALLOUT
  &&L_CALLOUT_CONTENTS,
  &&L_CALLOUT_NAME,
#endif
  };
#endif

  int i, n, num_mem, best_len, pop_level;
  LengthType tlen, tlen2;
  MemNumType mem;
  RelAddrType addr;
  UChar *s, *q, *ps, *sbegin;
  UChar *right_range;
  int is_alloca;
  char *alloc_base;
  StackType *stk_base, *stk, *stk_end;
  StackType *stkp; /* used as any purpose. */
  StackIndex si;
  StackIndex *repeat_stk;
  StackIndex *mem_start_stk, *mem_end_stk;
  UChar* keep;
#ifdef USE_RETRY_LIMIT_IN_MATCH
  unsigned long retry_limit_in_match;
  unsigned long retry_in_match_counter;
#endif

#ifdef USE_CALLOUT
  int of;
#endif

  Operation* p = reg->ops;
  OnigOptionType option = reg->options;
  OnigEncoding encode = reg->enc;
  OnigCaseFoldType case_fold_flag = reg->case_fold_flag;

#ifdef ONIG_DEBUG_MATCH
  static unsigned int counter = 1;
#endif

#ifdef USE_DIRECT_THREADED_CODE
  if (IS_NULL(msa)) {
    for (i = 0; i < reg->ops_used; i++) {
       const void* addr;
       addr = opcode_to_label[p->opcode];
       p->opaddr = addr;
       p++;
    }
    return ONIG_NORMAL;
  }
#endif

#ifdef USE_CALLOUT
  msa->mp->match_at_call_counter++;
#endif

#ifdef USE_RETRY_LIMIT_IN_MATCH
  retry_limit_in_match = msa->retry_limit_in_match;
#endif

  pop_level = reg->stack_pop_level;
  num_mem = reg->num_mem;
  STACK_INIT(INIT_MATCH_STACK_SIZE);
  UPDATE_FOR_STACK_REALLOC;
  for (i = 1; i <= num_mem; i++) {
    mem_start_stk[i] = mem_end_stk[i] = INVALID_STACK_INDEX;
  }

#ifdef ONIG_DEBUG_MATCH
  fprintf(stderr, "match_at: str: %p, end: %p, start: %p, sprev: %p\n",
          str, end, sstart, sprev);
  fprintf(stderr, "size: %d, start offset: %d\n",
          (int )(end - str), (int )(sstart - str));
#endif

  best_len = ONIG_MISMATCH;
  keep = s = (UChar* )sstart;
  STACK_PUSH_BOTTOM(STK_ALT, FinishCode);  /* bottom stack */
  INIT_RIGHT_RANGE;

#ifdef USE_RETRY_LIMIT_IN_MATCH
  retry_in_match_counter = 0;
#endif

  BYTECODE_INTERPRETER_START {
    CASE_OP(END)
      n = (int )(s - sstart);
      if (n > best_len) {
        OnigRegion* region;
#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
        if (IS_FIND_LONGEST(option)) {
          if (n > msa->best_len) {
            msa->best_len = n;
            msa->best_s   = (UChar* )sstart;
          }
          else
            goto end_best_len;
        }
#endif
        best_len = n;
        region = msa->region;
        if (region) {
          if (keep > s) keep = s;

#ifdef USE_POSIX_API_REGION_OPTION
          if (IS_POSIX_REGION(msa->options)) {
            posix_regmatch_t* rmt = (posix_regmatch_t* )region;

            rmt[0].rm_so = (regoff_t )(keep - str);
            rmt[0].rm_eo = (regoff_t )(s    - str);
            for (i = 1; i <= num_mem; i++) {
              if (mem_end_stk[i] != INVALID_STACK_INDEX) {
                if (MEM_STATUS_AT(reg->bt_mem_start, i))
                  rmt[i].rm_so = (regoff_t )(STACK_AT(mem_start_stk[i])->u.mem.pstr - str);
                else
                  rmt[i].rm_so = (regoff_t )((UChar* )((void* )(mem_start_stk[i])) - str);

                rmt[i].rm_eo = (regoff_t )((MEM_STATUS_AT(reg->bt_mem_end, i)
                                            ? STACK_AT(mem_end_stk[i])->u.mem.pstr
                                            : (UChar* )((void* )mem_end_stk[i]))
                                           - str);
              }
              else {
                rmt[i].rm_so = rmt[i].rm_eo = ONIG_REGION_NOTPOS;
              }
            }
          }
          else {
#endif /* USE_POSIX_API_REGION_OPTION */
            region->beg[0] = (int )(keep - str);
            region->end[0] = (int )(s    - str);
            for (i = 1; i <= num_mem; i++) {
              if (mem_end_stk[i] != INVALID_STACK_INDEX) {
                if (MEM_STATUS_AT(reg->bt_mem_start, i))
                  region->beg[i] = (int )(STACK_AT(mem_start_stk[i])->u.mem.pstr - str);
                else
                  region->beg[i] = (int )((UChar* )((void* )mem_start_stk[i]) - str);

                region->end[i] = (int )((MEM_STATUS_AT(reg->bt_mem_end, i)
                                         ? STACK_AT(mem_end_stk[i])->u.mem.pstr
                                         : (UChar* )((void* )mem_end_stk[i])) - str);
              }
              else {
                region->beg[i] = region->end[i] = ONIG_REGION_NOTPOS;
              }
            }

#ifdef USE_CAPTURE_HISTORY
            if (reg->capture_history != 0) {
              int r;
              OnigCaptureTreeNode* node;

              if (IS_NULL(region->history_root)) {
                region->history_root = node = history_node_new();
                CHECK_NULL_RETURN_MEMERR(node);
              }
              else {
                node = region->history_root;
                history_tree_clear(node);
              }

              node->group = 0;
              node->beg   = (int )(keep - str);
              node->end   = (int )(s    - str);

              stkp = stk_base;
              r = make_capture_history_tree(region->history_root, &stkp,
                                            stk, (UChar* )str, reg);
              if (r < 0) {
                best_len = r; /* error code */
                goto finish;
              }
            }
#endif /* USE_CAPTURE_HISTORY */
#ifdef USE_POSIX_API_REGION_OPTION
          } /* else IS_POSIX_REGION() */
#endif
        } /* if (region) */
      } /* n > best_len */

#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
    end_best_len:
#endif
      SOP_OUT;

      if (IS_FIND_CONDITION(option)) {
        if (IS_FIND_NOT_EMPTY(option) && s == sstart) {
          best_len = ONIG_MISMATCH;
          goto fail; /* for retry */
        }
        if (IS_FIND_LONGEST(option) && DATA_ENSURE_CHECK1) {
          goto fail; /* for retry */
        }
      }

      /* default behavior: return first-matching result. */
      goto finish;

    CASE_OP(EXACT1)
      DATA_ENSURE(1);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      s++;
      INC_OP;
      NEXT_OUT;

    CASE_OP(EXACT1_IC)
      {
        int len;
        UChar *q, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

        DATA_ENSURE(1);
        len = ONIGENC_MBC_CASE_FOLD(encode,
                 /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
                                    case_fold_flag,
                                    &s, end, lowbuf);
        DATA_ENSURE(0);
        q = lowbuf;
        ps = p->exact.s;
        while (len-- > 0) {
          if (*ps != *q) goto fail;
          ps++; q++;
        }
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(EXACT2)
      DATA_ENSURE(2);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACT3)
      DATA_ENSURE(3);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACT4)
      DATA_ENSURE(4);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACT5)
      DATA_ENSURE(5);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      sprev = s;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACTN)
      tlen = p->exact_n.n;
      DATA_ENSURE(tlen);
      ps = p->exact_n.s;
      while (tlen-- > 0) {
        if (*ps++ != *s++) goto fail;
      }
      sprev = s - 1;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACTN_IC)
      {
        int len;
        UChar *q, *endp, lowbuf[ONIGENC_MBC_CASE_FOLD_MAXLEN];

        tlen = p->exact_n.n;
        ps   = p->exact_n.s;
        endp = ps + tlen;
        while (ps < endp) {
          sprev = s;
          DATA_ENSURE(1);
          len = ONIGENC_MBC_CASE_FOLD(encode,
                        /* DISABLE_CASE_FOLD_MULTI_CHAR(case_fold_flag), */
                                      case_fold_flag,
                                      &s, end, lowbuf);
          DATA_ENSURE(0);
          q = lowbuf;
          while (len-- > 0) {
            if (*ps != *q) goto fail;
            ps++; q++;
          }
        }
      }

      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACTMB2N1)
      DATA_ENSURE(2);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      s++;
      INC_OP;
      NEXT_OUT;

    CASE_OP(EXACTMB2N2)
      DATA_ENSURE(4);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      sprev = s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACTMB2N3)
      DATA_ENSURE(6);
      ps = p->exact.s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      sprev = s;
      if (*ps != *s) goto fail;
      ps++; s++;
      if (*ps != *s) goto fail;
      ps++; s++;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACTMB2N)
      tlen = p->exact_n.n;
      DATA_ENSURE(tlen * 2);
      ps = p->exact_n.s;
      while (tlen-- > 0) {
        if (*ps != *s) goto fail;
        ps++; s++;
        if (*ps != *s) goto fail;
        ps++; s++;
      }
      sprev = s - 2;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACTMB3N)
      tlen = p->exact_n.n;
      DATA_ENSURE(tlen * 3);
      ps = p->exact_n.s;
      while (tlen-- > 0) {
        if (*ps != *s) goto fail;
        ps++; s++;
        if (*ps != *s) goto fail;
        ps++; s++;
        if (*ps != *s) goto fail;
        ps++; s++;
      }
      sprev = s - 3;
      INC_OP;
      JUMP_OUT;

    CASE_OP(EXACTMBN)
      tlen  = p->exact_len_n.len; /* mb byte len */
      tlen2 = p->exact_len_n.n;   /* number of chars */
      tlen2 *= tlen;
      DATA_ENSURE(tlen2);
      ps = p->exact_len_n.s;
      while (tlen2-- > 0) {
        if (*ps != *s) goto fail;
        ps++; s++;
      }
      sprev = s - tlen;
      INC_OP;
      JUMP_OUT;

    CASE_OP(CCLASS)
      DATA_ENSURE(1);
      if (BITSET_AT(p->cclass.bsp, *s) == 0) goto fail;
      s++;
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MB)
      if (! ONIGENC_IS_MBC_HEAD(encode, s)) goto fail;

    cclass_mb:
      {
        OnigCodePoint code;
        UChar *ss;
        int mb_len;

        DATA_ENSURE(1);
        mb_len = enclen(encode, s);
        DATA_ENSURE(mb_len);
        ss = s;
        s += mb_len;
        code = ONIGENC_MBC_TO_CODE(encode, ss, s);
        if (! onig_is_in_code_range(p->cclass_mb.mb, code)) goto fail;
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MIX)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s)) {
        goto cclass_mb;
      }
      else {
        if (BITSET_AT(p->cclass_mix.bsp, *s) == 0)
          goto fail;

        s++;
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_NOT)
      DATA_ENSURE(1);
      if (BITSET_AT(p->cclass.bsp, *s) != 0) goto fail;
      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MB_NOT)
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_HEAD(encode, s)) {
        s++;
        goto cc_mb_not_success;
      }

    cclass_mb_not:
      {
        OnigCodePoint code;
        UChar *ss;
        int mb_len = enclen(encode, s);

        if (! DATA_ENSURE_CHECK(mb_len)) {
          DATA_ENSURE(1);
          s = (UChar* )end;
          goto cc_mb_not_success;
        }

        ss = s;
        s += mb_len;
        code = ONIGENC_MBC_TO_CODE(encode, ss, s);
        if (onig_is_in_code_range(p->cclass_mb.mb, code)) goto fail;
      }

    cc_mb_not_success:
      INC_OP;
      NEXT_OUT;

    CASE_OP(CCLASS_MIX_NOT)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_HEAD(encode, s)) {
        goto cclass_mb_not;
      }
      else {
        if (BITSET_AT(p->cclass_mix.bsp, *s) != 0)
          goto fail;

        s++;
      }
      INC_OP;
      NEXT_OUT;

    CASE_OP(ANYCHAR)
      DATA_ENSURE(1);
      n = enclen(encode, s);
      DATA_ENSURE(n);
      if (ONIGENC_IS_MBC_NEWLINE(encode, s, end)) goto fail;
      s += n;
      INC_OP;
      NEXT_OUT;

    CASE_OP(ANYCHAR_ML)
      DATA_ENSURE(1);
      n = enclen(encode, s);
      DATA_ENSURE(n);
      s += n;
      INC_OP;
      NEXT_OUT;

    CASE_OP(ANYCHAR_STAR)
      INC_OP;
      while (DATA_ENSURE_CHECK1) {
        STACK_PUSH_ALT(p, s, sprev);
        n = enclen(encode, s);
        DATA_ENSURE(n);
        if (ONIGENC_IS_MBC_NEWLINE(encode, s, end))  goto fail;
        sprev = s;
        s += n;
      }
      JUMP_OUT;

    CASE_OP(ANYCHAR_ML_STAR)
      INC_OP;
      while (DATA_ENSURE_CHECK1) {
        STACK_PUSH_ALT(p, s, sprev);
        n = enclen(encode, s);
        if (n > 1) {
          DATA_ENSURE(n);
          sprev = s;
          s += n;
        }
        else {
          sprev = s;
          s++;
        }
      }
      JUMP_OUT;

    CASE_OP(ANYCHAR_STAR_PEEK_NEXT)
      {
        UChar c;

        c = p->anychar_star_peek_next.c;
        INC_OP;
        while (DATA_ENSURE_CHECK1) {
          if (c == *s) {
            STACK_PUSH_ALT(p, s, sprev);
          }
          n = enclen(encode, s);
          DATA_ENSURE(n);
          if (ONIGENC_IS_MBC_NEWLINE(encode, s, end))  goto fail;
          sprev = s;
          s += n;
        }
      }
      NEXT_OUT;

    CASE_OP(ANYCHAR_ML_STAR_PEEK_NEXT)
      {
        UChar c;

        c = p->anychar_star_peek_next.c;
        INC_OP;
        while (DATA_ENSURE_CHECK1) {
          if (c == *s) {
            STACK_PUSH_ALT(p, s, sprev);
          }
          n = enclen(encode, s);
          if (n > 1) {
            DATA_ENSURE(n);
            sprev = s;
            s += n;
          }
          else {
            sprev = s;
            s++;
          }
        }
      }
      NEXT_OUT;

    CASE_OP(WORD)
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_WORD(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(WORD_ASCII)
      DATA_ENSURE(1);
      if (! ONIGENC_IS_MBC_WORD_ASCII(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(NO_WORD)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_WORD(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(NO_WORD_ASCII)
      DATA_ENSURE(1);
      if (ONIGENC_IS_MBC_WORD_ASCII(encode, s, end))
        goto fail;

      s += enclen(encode, s);
      INC_OP;
      NEXT_OUT;

    CASE_OP(WORD_BOUNDARY)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (ON_STR_BEGIN(s)) {
          DATA_ENSURE(1);
          if (! IS_MBC_WORD_ASCII_MODE(encode, s, end, mode))
            goto fail;
        }
        else if (ON_STR_END(s)) {
          if (! IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
        else {
          if (IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)
              == IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(NO_WORD_BOUNDARY)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (ON_STR_BEGIN(s)) {
          if (DATA_ENSURE_CHECK1 && IS_MBC_WORD_ASCII_MODE(encode, s, end, mode))
            goto fail;
        }
        else if (ON_STR_END(s)) {
          if (IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
        else {
          if (IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)
              != IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode))
            goto fail;
        }
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_WORD_BEGIN_END
    CASE_OP(WORD_BEGIN)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (DATA_ENSURE_CHECK1 && IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)) {
          if (ON_STR_BEGIN(s) || !IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode)) {
            INC_OP;
            JUMP_OUT;
          }
        }
      }
      goto fail;

    CASE_OP(WORD_END)
      {
        ModeType mode;

        mode = p->word_boundary.mode;
        if (!ON_STR_BEGIN(s) && IS_MBC_WORD_ASCII_MODE(encode, sprev, end, mode)) {
          if (ON_STR_END(s) || ! IS_MBC_WORD_ASCII_MODE(encode, s, end, mode)) {
            INC_OP;
            JUMP_OUT;
          }
        }
      }
      goto fail;
#endif

    CASE_OP(EXTENDED_GRAPHEME_CLUSTER_BOUNDARY)
      if (onigenc_egcb_is_break_position(encode, s, sprev, str, end)) {
        INC_OP;
        JUMP_OUT;
      }
      goto fail;

    CASE_OP(NO_EXTENDED_GRAPHEME_CLUSTER_BOUNDARY)
      if (onigenc_egcb_is_break_position(encode, s, sprev, str, end))
        goto fail;

      INC_OP;
      JUMP_OUT;

    CASE_OP(BEGIN_BUF)
      if (! ON_STR_BEGIN(s)) goto fail;

      INC_OP;
      JUMP_OUT;

    CASE_OP(END_BUF)
      if (! ON_STR_END(s)) goto fail;

      INC_OP;
      JUMP_OUT;

    CASE_OP(BEGIN_LINE)
      if (ON_STR_BEGIN(s)) {
        if (IS_NOTBOL(msa->options)) goto fail;
        INC_OP;
        JUMP_OUT;
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, sprev, end) && !ON_STR_END(s)) {
        INC_OP;
        JUMP_OUT;
      }
      goto fail;

    CASE_OP(END_LINE)
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE(encode, sprev, end)) {
#endif
          if (IS_NOTEOL(msa->options)) goto fail;
          INC_OP;
          JUMP_OUT;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        }
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, s, end)) {
        INC_OP;
        JUMP_OUT;
      }
#ifdef USE_CRNL_AS_LINE_TERMINATOR
      else if (ONIGENC_IS_MBC_CRNL(encode, s, end)) {
        INC_OP;
        JUMP_OUT;
      }
#endif
      goto fail;

    CASE_OP(SEMI_END_BUF)
      if (ON_STR_END(s)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        if (IS_EMPTY_STR || !ONIGENC_IS_MBC_NEWLINE(encode, sprev, end)) {
#endif
          if (IS_NOTEOL(msa->options)) goto fail;
          INC_OP;
          JUMP_OUT;
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
        }
#endif
      }
      else if (ONIGENC_IS_MBC_NEWLINE(encode, s, end) &&
               ON_STR_END(s + enclen(encode, s))) {
        INC_OP;
        JUMP_OUT;
      }
#ifdef USE_CRNL_AS_LINE_TERMINATOR
      else if (ONIGENC_IS_MBC_CRNL(encode, s, end)) {
        UChar* ss = s + enclen(encode, s);
        ss += enclen(encode, ss);
        if (ON_STR_END(ss)) {
          INC_OP;
          JUMP_OUT;
        }
      }
#endif
      goto fail;

    CASE_OP(BEGIN_POSITION)
      if (s != msa->start)
        goto fail;

      INC_OP;
      JUMP_OUT;

    CASE_OP(MEMORY_START_PUSH)
      mem = p->memory_start.num;
      STACK_PUSH_MEM_START(mem, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(MEMORY_START)
      mem = p->memory_start.num;
      mem_start_stk[mem] = (StackIndex )((void* )s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(MEMORY_END_PUSH)
      mem = p->memory_end.num;
      STACK_PUSH_MEM_END(mem, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(MEMORY_END)
      mem = p->memory_end.num;
      mem_end_stk[mem] = (StackIndex )((void* )s);
      INC_OP;
      JUMP_OUT;

#ifdef USE_CALL
    CASE_OP(MEMORY_END_PUSH_REC)
      mem = p->memory_end.num;
      STACK_GET_MEM_START(mem, stkp); /* should be before push mem-end. */
      STACK_PUSH_MEM_END(mem, s);
      mem_start_stk[mem] = GET_STACK_INDEX(stkp);
      INC_OP;
      JUMP_OUT;

    CASE_OP(MEMORY_END_REC)
      mem = p->memory_end.num;
      mem_end_stk[mem] = (StackIndex )((void* )s);
      STACK_GET_MEM_START(mem, stkp);

      if (MEM_STATUS_AT(reg->bt_mem_start, mem))
        mem_start_stk[mem] = GET_STACK_INDEX(stkp);
      else
        mem_start_stk[mem] = (StackIndex )((void* )stkp->u.mem.pstr);

      STACK_PUSH_MEM_END_MARK(mem);
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(BACKREF1)
      mem = 1;
      goto backref;

    CASE_OP(BACKREF2)
      mem = 2;
      goto backref;

    CASE_OP(BACKREF_N)
      mem = p->backref_n.n1;
    backref:
      {
        int len;
        UChar *pstart, *pend;

        if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
        if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

        if (MEM_STATUS_AT(reg->bt_mem_start, mem))
          pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
        else
          pstart = (UChar* )((void* )mem_start_stk[mem]);

        pend = (MEM_STATUS_AT(reg->bt_mem_end, mem)
                ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                : (UChar* )((void* )mem_end_stk[mem]));
        n = (int )(pend - pstart);
        DATA_ENSURE(n);
        sprev = s;
        STRING_CMP(pstart, s, n);
        while (sprev + (len = enclen(encode, sprev)) < s)
          sprev += len;
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(BACKREF_N_IC)
      mem = p->backref_n.n1;
      {
        int len;
        UChar *pstart, *pend;

        if (mem_end_stk[mem]   == INVALID_STACK_INDEX) goto fail;
        if (mem_start_stk[mem] == INVALID_STACK_INDEX) goto fail;

        if (MEM_STATUS_AT(reg->bt_mem_start, mem))
          pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
        else
          pstart = (UChar* )((void* )mem_start_stk[mem]);

        pend = (MEM_STATUS_AT(reg->bt_mem_end, mem)
                ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                : (UChar* )((void* )mem_end_stk[mem]));
        n = (int )(pend - pstart);
        DATA_ENSURE(n);
        sprev = s;
        STRING_CMP_IC(case_fold_flag, pstart, &s, n);
        while (sprev + (len = enclen(encode, sprev)) < s)
          sprev += len;
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(BACKREF_MULTI)
      {
        int len, is_fail;
        UChar *pstart, *pend, *swork;

        tlen = p->backref_general.num;
        for (i = 0; i < tlen; i++) {
          mem = tlen == 1 ? p->backref_general.n1 : p->backref_general.ns[i];

          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

          if (MEM_STATUS_AT(reg->bt_mem_start, mem))
            pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
          else
            pstart = (UChar* )((void* )mem_start_stk[mem]);

          pend = (MEM_STATUS_AT(reg->bt_mem_end, mem)
                  ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                  : (UChar* )((void* )mem_end_stk[mem]));
          n = (int )(pend - pstart);
          DATA_ENSURE(n);
          sprev = s;
          swork = s;
          STRING_CMP_VALUE(pstart, swork, n, is_fail);
          if (is_fail) continue;
          s = swork;
          while (sprev + (len = enclen(encode, sprev)) < s)
            sprev += len;

          break; /* success */
        }
        if (i == tlen) goto fail;
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(BACKREF_MULTI_IC)
      {
        int len, is_fail;
        UChar *pstart, *pend, *swork;

        tlen = p->backref_general.num;
        for (i = 0; i < tlen; i++) {
          mem = tlen == 1 ? p->backref_general.n1 : p->backref_general.ns[i];

          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;

          if (MEM_STATUS_AT(reg->bt_mem_start, mem))
            pstart = STACK_AT(mem_start_stk[mem])->u.mem.pstr;
          else
            pstart = (UChar* )((void* )mem_start_stk[mem]);

          pend = (MEM_STATUS_AT(reg->bt_mem_end, mem)
                  ? STACK_AT(mem_end_stk[mem])->u.mem.pstr
                  : (UChar* )((void* )mem_end_stk[mem]));
          n = (int )(pend - pstart);
          DATA_ENSURE(n);
          sprev = s;
          swork = s;
          STRING_CMP_VALUE_IC(case_fold_flag, pstart, &swork, n, is_fail);
          if (is_fail) continue;
          s = swork;
          while (sprev + (len = enclen(encode, sprev)) < s)
            sprev += len;

          break; /* success */
        }
        if (i == tlen) goto fail;
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_BACKREF_WITH_LEVEL
    CASE_OP(BACKREF_WITH_LEVEL_IC)
      n = 1; /* ignore case */
      goto backref_with_level;
    CASE_OP(BACKREF_WITH_LEVEL)
      {
        int len;
        int level;
        MemNumType* mems;

        n = 0;
      backref_with_level:
        level = p->backref_general.nest_level;
        tlen  = p->backref_general.num;
        mems = tlen == 1 ? &(p->backref_general.n1) : p->backref_general.ns;

        sprev = s;
        if (backref_match_at_nested_level(reg, stk, stk_base, n,
                    case_fold_flag, level, (int )tlen, mems, &s, end)) {
          if (sprev < end) {
            while (sprev + (len = enclen(encode, sprev)) < s)
              sprev += len;
          }
        }
        else
          goto fail;
      }
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(BACKREF_CHECK)
      {
        MemNumType* mems;

        tlen  = p->backref_general.num;
        mems = tlen == 1 ? &(p->backref_general.n1) : p->backref_general.ns;

        for (i = 0; i < tlen; i++) {
          mem = mems[i];
          if (mem_end_stk[mem]   == INVALID_STACK_INDEX) continue;
          if (mem_start_stk[mem] == INVALID_STACK_INDEX) continue;
          break; /* success */
        }
        if (i == tlen) goto fail;
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_BACKREF_WITH_LEVEL
    CASE_OP(BACKREF_CHECK_WITH_LEVEL)
      {
        LengthType level;
        MemNumType* mems;

        level = p->backref_general.nest_level;
        tlen  = p->backref_general.num;
        mems = tlen == 1 ? &(p->backref_general.n1) : p->backref_general.ns;

        if (backref_check_at_nested_level(reg, stk, stk_base,
                                          (int )level, (int )tlen, mems) != 0) {
        }
        else
          goto fail;
      }
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(EMPTY_CHECK_START)
      mem = p->empty_check_start.mem;   /* mem: null check id */
      STACK_PUSH_EMPTY_CHECK_START(mem, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(EMPTY_CHECK_END)
      {
        int is_empty;

        mem = p->empty_check_end.mem;  /* mem: null check id */
        STACK_EMPTY_CHECK(is_empty, mem, s);
        INC_OP;
        if (is_empty) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "EMPTY_CHECK_END: skip  id:%d, s:%p\n", (int )mem, s);
#endif
        empty_check_found:
          /* empty loop founded, skip next instruction */
#ifdef ONIG_DEBUG
          switch (p->opcode) {
          case OP_JUMP:
          case OP_PUSH:
          case OP_REPEAT_INC:
          case OP_REPEAT_INC_NG:
          case OP_REPEAT_INC_SG:
          case OP_REPEAT_INC_NG_SG:
            INC_OP;
            break;
          default:
            goto unexpected_bytecode_error;
            break;
          }
#else
          INC_OP;
#endif
        }
      }
      JUMP_OUT;

#ifdef USE_INSISTENT_CHECK_CAPTURES_IN_EMPTY_REPEAT
    CASE_OP(EMPTY_CHECK_END_MEMST)
      {
        int is_empty;

        mem = p->empty_check_end.mem;  /* mem: null check id */
        STACK_EMPTY_CHECK_MEM(is_empty, mem, s, reg);
        INC_OP;
        if (is_empty) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "EMPTY_CHECK_END_MEM: skip  id:%d, s:%p\n", (int)mem, s);
#endif
          if (is_empty == -1) goto fail;
          goto empty_check_found;
        }
      }
      JUMP_OUT;
#endif

#ifdef USE_CALL
    CASE_OP(EMPTY_CHECK_END_MEMST_PUSH)
      {
        int is_empty;

        mem = p->empty_check_end.mem;  /* mem: null check id */
#ifdef USE_INSISTENT_CHECK_CAPTURES_IN_EMPTY_REPEAT
        STACK_EMPTY_CHECK_MEM_REC(is_empty, mem, s, reg);
#else
        STACK_EMPTY_CHECK_REC(is_empty, mem, s);
#endif
        INC_OP;
        if (is_empty) {
#ifdef ONIG_DEBUG_MATCH
          fprintf(stderr, "EMPTY_CHECK_END_MEM_PUSH: skip  id:%d, s:%p\n",
                  (int )mem, s);
#endif
          if (is_empty == -1) goto fail;
          goto empty_check_found;
        }
        else {
          STACK_PUSH_EMPTY_CHECK_END(mem);
        }
      }
      JUMP_OUT;
#endif

    CASE_OP(JUMP)
      addr = p->jump.addr;
      p += addr;
      CHECK_INTERRUPT_JUMP_OUT;

    CASE_OP(PUSH)
      addr = p->push.addr;
      STACK_PUSH_ALT(p + addr, s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(PUSH_SUPER)
      addr = p->push.addr;
      STACK_PUSH_SUPER_ALT(p + addr, s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(POP_OUT)
      STACK_POP_ONE;
      /* for stop backtrack */
      /* CHECK_RETRY_LIMIT_IN_MATCH; */
      INC_OP;
      JUMP_OUT;

 #ifdef USE_OP_PUSH_OR_JUMP_EXACT
    CASE_OP(PUSH_OR_JUMP_EXACT1)
      {
        UChar c;

        addr = p->push_or_jump_exact1.addr;
        c    = p->push_or_jump_exact1.c;
        if (DATA_ENSURE_CHECK1 && c == *s) {
          STACK_PUSH_ALT(p + addr, s, sprev);
          INC_OP;
          JUMP_OUT;
        }
      }
      p += addr;
      JUMP_OUT;
#endif

    CASE_OP(PUSH_IF_PEEK_NEXT)
      {
        UChar c;

        addr = p->push_if_peek_next.addr;
        c    = p->push_if_peek_next.c;
        if (c == *s) {
          STACK_PUSH_ALT(p + addr, s, sprev);
          INC_OP;
          JUMP_OUT;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(REPEAT)
      mem  = p->repeat.id;  /* mem: OP_REPEAT ID */
      addr = p->repeat.addr;

      STACK_ENSURE(1);
      repeat_stk[mem] = GET_STACK_INDEX(stk);
      STACK_PUSH_REPEAT(mem, p + 1);

      if (reg->repeat_range[mem].lower == 0) {
        STACK_PUSH_ALT(p + addr, s, sprev);
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(REPEAT_NG)
      mem  = p->repeat.id;  /* mem: OP_REPEAT ID */
      addr = p->repeat.addr;

      STACK_ENSURE(1);
      repeat_stk[mem] = GET_STACK_INDEX(stk);
      STACK_PUSH_REPEAT(mem, p + 1);

      if (reg->repeat_range[mem].lower == 0) {
        STACK_PUSH_ALT(p + 1, s, sprev);
        p += addr;
      }
      else
        INC_OP;
      JUMP_OUT;

    CASE_OP(REPEAT_INC)
      mem  = p->repeat_inc.id;  /* mem: OP_REPEAT ID */
      si   = repeat_stk[mem];
      stkp = STACK_AT(si);

    repeat_inc:
      stkp->u.repeat.count++;
      if (stkp->u.repeat.count >= reg->repeat_range[mem].upper) {
        /* end of repeat. Nothing to do. */
        INC_OP;
      }
      else if (stkp->u.repeat.count >= reg->repeat_range[mem].lower) {
        INC_OP;
        STACK_PUSH_ALT(p, s, sprev);
        p = STACK_AT(si)->u.repeat.pcode; /* Don't use stkp after PUSH. */
      }
      else {
        p = stkp->u.repeat.pcode;
      }
      STACK_PUSH_REPEAT_INC(si);
      CHECK_INTERRUPT_JUMP_OUT;

    CASE_OP(REPEAT_INC_SG)
      mem = p->repeat_inc.id;  /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT(mem, stkp);
      si = GET_STACK_INDEX(stkp);
      goto repeat_inc;

    CASE_OP(REPEAT_INC_NG)
      mem = p->repeat_inc.id;  /* mem: OP_REPEAT ID */
      si = repeat_stk[mem];
      stkp = STACK_AT(si);

    repeat_inc_ng:
      stkp->u.repeat.count++;
      if (stkp->u.repeat.count < reg->repeat_range[mem].upper) {
        if (stkp->u.repeat.count >= reg->repeat_range[mem].lower) {
          Operation* pcode = stkp->u.repeat.pcode;

          STACK_PUSH_REPEAT_INC(si);
          STACK_PUSH_ALT(pcode, s, sprev);
          INC_OP;
        }
        else {
          p = stkp->u.repeat.pcode;
          STACK_PUSH_REPEAT_INC(si);
        }
      }
      else if (stkp->u.repeat.count == reg->repeat_range[mem].upper) {
        STACK_PUSH_REPEAT_INC(si);
        INC_OP;
      }
      CHECK_INTERRUPT_JUMP_OUT;

    CASE_OP(REPEAT_INC_NG_SG)
      mem = p->repeat_inc.id;  /* mem: OP_REPEAT ID */
      STACK_GET_REPEAT(mem, stkp);
      si = GET_STACK_INDEX(stkp);
      goto repeat_inc_ng;

    CASE_OP(PREC_READ_START)
      STACK_PUSH_POS(s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(PREC_READ_END)
      STACK_EXEC_TO_VOID(stkp);
      s     = stkp->u.state.pstr;
      sprev = stkp->u.state.pstr_prev;
      INC_OP;
      JUMP_OUT;

    CASE_OP(PREC_READ_NOT_START)
      addr = p->prec_read_not_start.addr;
      STACK_PUSH_ALT_PREC_READ_NOT(p + addr, s, sprev);
      INC_OP;
      JUMP_OUT;

    CASE_OP(PREC_READ_NOT_END)
      STACK_POP_TIL_ALT_PREC_READ_NOT;
      goto fail;

    CASE_OP(ATOMIC_START)
      STACK_PUSH_TO_VOID_START;
      INC_OP;
      JUMP_OUT;

    CASE_OP(ATOMIC_END)
      STACK_EXEC_TO_VOID(stkp);
      INC_OP;
      JUMP_OUT;

    CASE_OP(LOOK_BEHIND)
      tlen = p->look_behind.len;
      s = (UChar* )ONIGENC_STEP_BACK(encode, str, s, (int )tlen);
      if (IS_NULL(s)) goto fail;
      sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s);
      INC_OP;
      JUMP_OUT;

    CASE_OP(LOOK_BEHIND_NOT_START)
      addr = p->look_behind_not_start.addr;
      tlen = p->look_behind_not_start.len;
      q = (UChar* )ONIGENC_STEP_BACK(encode, str, s, (int )tlen);
      if (IS_NULL(q)) {
        /* too short case -> success. ex. /(?<!XXX)a/.match("a")
           If you want to change to fail, replace following line. */
        p += addr;
        /* goto fail; */
      }
      else {
        STACK_PUSH_ALT_LOOK_BEHIND_NOT(p + addr, s, sprev);
        s = q;
        sprev = (UChar* )onigenc_get_prev_char_head(encode, str, s);
        INC_OP;
      }
      JUMP_OUT;

    CASE_OP(LOOK_BEHIND_NOT_END)
      STACK_POP_TIL_ALT_LOOK_BEHIND_NOT;
      INC_OP;
      goto fail;

#ifdef USE_CALL
    CASE_OP(CALL)
      addr = p->call.addr;
      INC_OP; STACK_PUSH_CALL_FRAME(p);
      p = reg->ops + addr;
      JUMP_OUT;

    CASE_OP(RETURN)
      STACK_RETURN(p);
      STACK_PUSH_RETURN;
      JUMP_OUT;
#endif

    CASE_OP(PUSH_SAVE_VAL)
      {
        SaveType type;

        type = p->push_save_val.type;
        mem  = p->push_save_val.id; /* mem: save id */
        switch ((enum SaveType )type) {
        case SAVE_KEEP:
          STACK_PUSH_SAVE_VAL(mem, type, s);
          break;

        case SAVE_S:
          STACK_PUSH_SAVE_VAL_WITH_SPREV(mem, type, s);
          break;

        case SAVE_RIGHT_RANGE:
          STACK_PUSH_SAVE_VAL(mem, SAVE_RIGHT_RANGE, right_range);
          break;
        }
      }
      INC_OP;
      JUMP_OUT;

    CASE_OP(UPDATE_VAR)
      {
        UpdateVarType type;
        enum SaveType save_type;

        type = p->update_var.type;
        mem  = p->update_var.id; /* mem: save id */

        switch ((enum UpdateVarType )type) {
        case UPDATE_VAR_KEEP_FROM_STACK_LAST:
          STACK_GET_SAVE_VAL_TYPE_LAST(SAVE_KEEP, keep);
          break;
        case UPDATE_VAR_S_FROM_STACK:
          STACK_GET_SAVE_VAL_TYPE_LAST_ID_WITH_SPREV(SAVE_S, mem, s);
          break;
        case UPDATE_VAR_RIGHT_RANGE_FROM_S_STACK:
          save_type = SAVE_S;
          goto get_save_val_type_last_id;
          break;
        case UPDATE_VAR_RIGHT_RANGE_FROM_STACK:
          save_type = SAVE_RIGHT_RANGE;
        get_save_val_type_last_id:
          STACK_GET_SAVE_VAL_TYPE_LAST_ID(save_type, mem, right_range);
          break;
        case UPDATE_VAR_RIGHT_RANGE_INIT:
          INIT_RIGHT_RANGE;
          break;
        }
      }
      INC_OP;
      JUMP_OUT;

#ifdef USE_CALLOUT
    CASE_OP(CALLOUT_CONTENTS)
      of = ONIG_CALLOUT_OF_CONTENTS;
      mem = p->callout_contents.num;
      goto callout_common_entry;
      BREAK_OUT;

    CASE_OP(CALLOUT_NAME)
      {
        int call_result;
        int name_id;
        int in;
        CalloutListEntry* e;
        OnigCalloutFunc func;
        OnigCalloutArgs args;

        of = ONIG_CALLOUT_OF_NAME;
        name_id = p->callout_name.id;
        mem     = p->callout_name.num;

      callout_common_entry:
        e = onig_reg_callout_list_at(reg, mem);
        in = e->in;
        if (of == ONIG_CALLOUT_OF_NAME) {
          func = onig_get_callout_start_func(reg, mem);
        }
        else {
          name_id = ONIG_NON_NAME_ID;
          func = msa->mp->progress_callout_of_contents;
        }

        if (IS_NOT_NULL(func) && (in & ONIG_CALLOUT_IN_PROGRESS) != 0) {
          CALLOUT_BODY(func, ONIG_CALLOUT_IN_PROGRESS, name_id,
                       (int )mem, msa->mp->callout_user_data, args, call_result);
          switch (call_result) {
          case ONIG_CALLOUT_FAIL:
            goto fail;
            break;
          case ONIG_CALLOUT_SUCCESS:
            goto retraction_callout2;
            break;
          default: /* error code */
            if (call_result > 0) {
              call_result = ONIGERR_INVALID_ARGUMENT;
            }
            best_len = call_result;
            goto finish;
            break;
          }
        }
        else {
        retraction_callout2:
          if ((in & ONIG_CALLOUT_IN_RETRACTION) != 0) {
            if (of == ONIG_CALLOUT_OF_NAME) {
              if (IS_NOT_NULL(func)) {
                STACK_PUSH_CALLOUT_NAME(name_id, mem, func);
              }
            }
            else {
              func = msa->mp->retraction_callout_of_contents;
              if (IS_NOT_NULL(func)) {
                STACK_PUSH_CALLOUT_CONTENTS(mem, func);
              }
            }
          }
        }
      }
      INC_OP;
      JUMP_OUT;
#endif

    CASE_OP(FINISH)
      goto finish;

#ifdef ONIG_DEBUG_STATISTICS
    fail:
      SOP_OUT;
      goto fail2;
#endif
    CASE_OP(FAIL)
#ifdef ONIG_DEBUG_STATISTICS
    fail2:
#else
    fail:
#endif
      STACK_POP;
      p     = stk->u.state.pcode;
      s     = stk->u.state.pstr;
      sprev = stk->u.state.pstr_prev;
      CHECK_RETRY_LIMIT_IN_MATCH;
      JUMP_OUT;

    DEFAULT_OP
      goto bytecode_error;

  } BYTECODE_INTERPRETER_END;

 finish:
  STACK_SAVE;
  return best_len;

#ifdef ONIG_DEBUG
 stack_error:
  STACK_SAVE;
  return ONIGERR_STACK_BUG;
#endif

 bytecode_error:
  STACK_SAVE;
  return ONIGERR_UNDEFINED_BYTECODE;

#ifdef ONIG_DEBUG
 unexpected_bytecode_error:
  STACK_SAVE;
  return ONIGERR_UNEXPECTED_BYTECODE;
#endif

#ifdef USE_RETRY_LIMIT_IN_MATCH
 retry_limit_in_match_over:
  STACK_SAVE;
  return ONIGERR_RETRY_LIMIT_IN_MATCH_OVER;
#endif
}