print_compiled_byte_code(FILE* f, regex_t* reg, int index,
                         Operation* start, OnigEncoding enc)
{
  int i, n;
  RelAddrType addr;
  LengthType  len;
  MemNumType  mem;
  OnigCodePoint code;
  ModeType mode;
  UChar *q;
  Operation* p;
  enum OpCode opcode;

  p = reg->ops + index;

#ifdef USE_DIRECT_THREADED_CODE
  opcode = reg->ocs[index];
#else
  opcode = p->opcode;
#endif

  fprintf(f, "%s", op2name(opcode));
  switch (opcode) {
  case OP_EXACT1:
    p_string(f, 1, p->exact.s); break;
  case OP_EXACT2:
    p_string(f, 2, p->exact.s); break;
  case OP_EXACT3:
    p_string(f, 3, p->exact.s); break;
  case OP_EXACT4:
    p_string(f, 4, p->exact.s); break;
  case OP_EXACT5:
    p_string(f, 5, p->exact.s); break;
  case OP_EXACTN:
    len = p->exact_n.n;
    p_string(f, len, p->exact_n.s); break;
  case OP_EXACTMB2N1:
    p_string(f, 2, p->exact.s); break;
  case OP_EXACTMB2N2:
    p_string(f, 4, p->exact.s); break;
  case OP_EXACTMB2N3:
    p_string(f, 3, p->exact.s); break;
  case OP_EXACTMB2N:
    len = p->exact_n.n;
    p_len_string(f, len, 2, p->exact_n.s); break;
  case OP_EXACTMB3N:
    len = p->exact_n.n;
    p_len_string(f, len, 3, p->exact_n.s); break;
  case OP_EXACTMBN:
    {
      int mb_len;

      mb_len = p->exact_len_n.len;
      len    = p->exact_len_n.n;
      q      = p->exact_len_n.s;
      fprintf(f, ":%d:%d:", mb_len, len);
      n = len * mb_len;
      while (n-- > 0) { fputc(*q++, f); }
    }
    break;
  case OP_EXACT1_IC:
    len = enclen(enc, p->exact.s);
    p_string(f, len, p->exact.s);
    break;
  case OP_EXACTN_IC:
    len = p->exact_n.n;
    p_len_string(f, len, 1, p->exact_n.s);
    break;

  case OP_CCLASS:
  case OP_CCLASS_NOT:
    n = bitset_on_num(p->cclass.bsp);
    fprintf(f, ":%d", n);
    break;
  case OP_CCLASS_MB:
  case OP_CCLASS_MB_NOT:
    {
      OnigCodePoint ncode;
      OnigCodePoint* codes;      

      codes = (OnigCodePoint* )p->cclass_mb.mb;
      GET_CODE_POINT(ncode, codes);
      codes++;
      GET_CODE_POINT(code, codes);
      fprintf(f, ":%u:%u", code, ncode);
    }
    break;
  case OP_CCLASS_MIX:
  case OP_CCLASS_MIX_NOT:
    {
      OnigCodePoint ncode;
      OnigCodePoint* codes;

      codes = (OnigCodePoint* )p->cclass_mix.mb;
      n = bitset_on_num(p->cclass_mix.bsp);

      GET_CODE_POINT(ncode, codes);
      codes++;
      GET_CODE_POINT(code, codes);
      fprintf(f, ":%d:%u:%u", n, code, ncode);
    }
    break;

  case OP_ANYCHAR_STAR_PEEK_NEXT:
  case OP_ANYCHAR_ML_STAR_PEEK_NEXT:
    p_string(f, 1, &(p->anychar_star_peek_next.c));
    break;

  case OP_WORD_BOUNDARY:
  case OP_NO_WORD_BOUNDARY:
  case OP_WORD_BEGIN:
  case OP_WORD_END:
    mode = p->word_boundary.mode;
    fprintf(f, ":%d", mode);
    break;

  case OP_BACKREF_N:
  case OP_BACKREF_N_IC:
    mem = p->backref_n.n1;
    fprintf(f, ":%d", mem);
    break;
  case OP_BACKREF_MULTI_IC:
  case OP_BACKREF_MULTI:
  case OP_BACKREF_CHECK:
    fputs(" ", f);
    n = p->backref_general.num;
    for (i = 0; i < n; i++) {
      mem = (n == 1) ? p->backref_general.n1 : p->backref_general.ns[i];
      if (i > 0) fputs(", ", f);
      fprintf(f, "%d", mem);
    }
    break;
  case OP_BACKREF_WITH_LEVEL:
  case OP_BACKREF_WITH_LEVEL_IC:
  case OP_BACKREF_CHECK_WITH_LEVEL:
    {
      LengthType level;

      level = p->backref_general.nest_level;
      fprintf(f, ":%d", level);
      fputs(" ", f);
      n = p->backref_general.num;
      for (i = 0; i < n; i++) {
        mem = (n == 1) ? p->backref_general.n1 : p->backref_general.ns[i];
        if (i > 0) fputs(", ", f);
        fprintf(f, "%d", mem);
      }
    }
    break;

  case OP_MEMORY_START:
  case OP_MEMORY_START_PUSH:
    mem = p->memory_start.num;
    fprintf(f, ":%d", mem);
    break;
  case OP_MEMORY_END_PUSH:
  case OP_MEMORY_END_PUSH_REC:
  case OP_MEMORY_END:
  case OP_MEMORY_END_REC:
    mem = p->memory_end.num;
    fprintf(f, ":%d", mem);
    break;

  case OP_JUMP:
    addr = p->jump.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    break;

  case OP_PUSH:
  case OP_PUSH_SUPER:
    addr = p->push.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    break;

#ifdef USE_OP_PUSH_OR_JUMP_EXACT
  case OP_PUSH_OR_JUMP_EXACT1:
    addr = p->push_or_jump_exact1.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    p_string(f, 1, &(p->push_or_jump_exact1.c));
    break;
#endif

  case OP_PUSH_IF_PEEK_NEXT:
    addr = p->push_if_peek_next.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    p_string(f, 1, &(p->push_if_peek_next.c));
    break;

  case OP_REPEAT:
  case OP_REPEAT_NG:
    mem = p->repeat.id;
    addr = p->repeat.addr;
    fprintf(f, ":%d:", mem);
    p_rel_addr(f, addr, p, start);
    break;

  case OP_REPEAT_INC:
  case OP_REPEAT_INC_NG:
  case OP_REPEAT_INC_SG:
  case OP_REPEAT_INC_NG_SG:
    mem = p->repeat.id;
    fprintf(f, ":%d", mem);
    break;

  case OP_EMPTY_CHECK_START:
    mem = p->empty_check_start.mem;
    fprintf(f, ":%d", mem);
    break;
  case OP_EMPTY_CHECK_END:
  case OP_EMPTY_CHECK_END_MEMST:
  case OP_EMPTY_CHECK_END_MEMST_PUSH:
    mem = p->empty_check_end.mem;
    fprintf(f, ":%d", mem);
    break;

  case OP_PREC_READ_NOT_START:
    addr = p->prec_read_not_start.addr;
    fputc(':', f);
    p_rel_addr(f, addr, p, start);
    break;

  case OP_LOOK_BEHIND:
    len = p->look_behind.len;
    fprintf(f, ":%d", len);
    break;

  case OP_LOOK_BEHIND_NOT_START:
    addr = p->look_behind_not_start.addr;
    len  = p->look_behind_not_start.len;
    fprintf(f, ":%d:", len);
    p_rel_addr(f, addr, p, start);
    break;

  case OP_CALL:
    addr = p->call.addr;
    fprintf(f, ":{/%d}", addr);
    break;

  case OP_PUSH_SAVE_VAL:
    {
      SaveType type;

      type = p->push_save_val.type;
      mem  = p->push_save_val.id;
      fprintf(f, ":%d:%d", type, mem);
    }
    break;

  case OP_UPDATE_VAR:
    {
      UpdateVarType type;

      type = p->update_var.type;
      mem  = p->update_var.id;
      fprintf(f, ":%d:%d", type, mem);
    }
    break;

#ifdef USE_CALLOUT
  case OP_CALLOUT_CONTENTS:
    mem = p->callout_contents.num;
    fprintf(f, ":%d", mem);
    break;

  case OP_CALLOUT_NAME:
    {
      int id;

      id  = p->callout_name.id;
      mem = p->callout_name.num;
      fprintf(f, ":%d:%d", id, mem);
    }
    break;
#endif

  case OP_TEXT_SEGMENT_BOUNDARY:
    if (p->text_segment_boundary.not != 0)
      fprintf(f, ":not");
    break;

  case OP_FINISH:
  case OP_END:
  case OP_ANYCHAR:
  case OP_ANYCHAR_ML:
  case OP_ANYCHAR_STAR:
  case OP_ANYCHAR_ML_STAR:
  case OP_WORD:
  case OP_WORD_ASCII:
  case OP_NO_WORD:
  case OP_NO_WORD_ASCII:
  case OP_BEGIN_BUF:
  case OP_END_BUF:
  case OP_BEGIN_LINE:
  case OP_END_LINE:
  case OP_SEMI_END_BUF:
  case OP_BEGIN_POSITION:
  case OP_BACKREF1:
  case OP_BACKREF2:
  case OP_FAIL:
  case OP_POP_OUT:
  case OP_PREC_READ_START:
  case OP_PREC_READ_END:
  case OP_PREC_READ_NOT_END:
  case OP_ATOMIC_START:
  case OP_ATOMIC_END:
  case OP_LOOK_BEHIND_NOT_END:
  case OP_RETURN:
    break;

  default:
    fprintf(stderr, "print_compiled_byte_code: undefined code %d\n", opcode);
    break;
  }
}