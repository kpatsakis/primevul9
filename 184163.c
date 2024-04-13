compile_string_node(Node* node, regex_t* reg)
{
  int r, len, prev_len, slen;
  UChar *p, *prev, *end;
  StrNode* sn;
  OnigEncoding enc = reg->enc;

  sn = STR_(node);
  if (sn->end <= sn->s)
    return 0;

  end = sn->end;

  p = prev = sn->s;
  prev_len = enclen(enc, p);
  p += prev_len;
  slen = 1;

  for (; p < end; ) {
    len = enclen(enc, p);
    if (len == prev_len) {
      slen++;
    }
    else {
      r = add_compile_string(prev, prev_len, slen, reg);
      if (r != 0) return r;

      prev  = p;
      slen  = 1;
      prev_len = len;
    }

    p += len;
  }

  return add_compile_string(prev, prev_len, slen, reg);
}