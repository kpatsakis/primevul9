add_compile_string(UChar* s, int mb_len, int str_len, regex_t* reg)
{
  int op;
  int r;
  int byte_len;
  UChar* p;
  UChar* end;

  op = select_str_opcode(mb_len, str_len);
  r = add_op(reg, op);
  if (r != 0) return r;

  byte_len = mb_len * str_len;
  end = s + byte_len;

  if (op == OP_STR_MBN) {
    p = onigenc_strdup(reg->enc, s, end);
    CHECK_NULL_RETURN_MEMERR(p);

    COP(reg)->exact_len_n.len = mb_len;
    COP(reg)->exact_len_n.n   = str_len;
    COP(reg)->exact_len_n.s   = p;
  }
  else if (IS_NEED_STR_LEN_OP(op)) {
    p = onigenc_strdup(reg->enc, s, end);
    CHECK_NULL_RETURN_MEMERR(p);
    COP(reg)->exact_n.n = str_len;
    COP(reg)->exact_n.s = p;
  }
  else {
    xmemset(COP(reg)->exact.s, 0, sizeof(COP(reg)->exact.s));
    xmemcpy(COP(reg)->exact.s, s, (size_t )byte_len);
  }

  return 0;
}