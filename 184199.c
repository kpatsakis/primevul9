compile_cclass_node(CClassNode* cc, regex_t* reg)
{
  int r;

  if (IS_NULL(cc->mbuf)) {
    r = add_op(reg, IS_NCCLASS_NOT(cc) ? OP_CCLASS_NOT : OP_CCLASS);
    if (r != 0) return r;

    COP(reg)->cclass.bsp = xmalloc(SIZE_BITSET);
    CHECK_NULL_RETURN_MEMERR(COP(reg)->cclass.bsp);
    xmemcpy(COP(reg)->cclass.bsp, cc->bs, SIZE_BITSET);
  }
  else {
    void* p;

    if (ONIGENC_MBC_MINLEN(reg->enc) > 1 || bitset_is_empty(cc->bs)) {
      r = add_op(reg, IS_NCCLASS_NOT(cc) ? OP_CCLASS_MB_NOT : OP_CCLASS_MB);
      if (r != 0) return r;

      p = set_multi_byte_cclass(cc->mbuf, reg);
      CHECK_NULL_RETURN_MEMERR(p);
      COP(reg)->cclass_mb.mb = p;
    }
    else {
      r = add_op(reg, IS_NCCLASS_NOT(cc) ? OP_CCLASS_MIX_NOT : OP_CCLASS_MIX);
      if (r != 0) return r;

      COP(reg)->cclass_mix.bsp = xmalloc(SIZE_BITSET);
      CHECK_NULL_RETURN_MEMERR(COP(reg)->cclass_mix.bsp);
      xmemcpy(COP(reg)->cclass_mix.bsp, cc->bs, SIZE_BITSET);

      p = set_multi_byte_cclass(cc->mbuf, reg);
      CHECK_NULL_RETURN_MEMERR(p);
      COP(reg)->cclass_mix.mb = p;
    }
  }

  return 0;
}