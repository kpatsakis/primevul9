compile_length_string_node(Node* node, regex_t* reg)
{
  int rlen, r, len, prev_len, slen;
  UChar *p, *prev;
  StrNode* sn;
  OnigEncoding enc = reg->enc;

  sn = STR_(node);
  if (sn->end <= sn->s)
    return 0;

  p = prev = sn->s;
  prev_len = enclen(enc, p);
  p += prev_len;
  slen = 1;
  rlen = 0;

  for (; p < sn->end; ) {
    len = enclen(enc, p);
    if (len == prev_len) {
      slen++;
    }
    else {
      r = add_compile_string_length(prev, prev_len, slen, reg);
      rlen += r;
      prev = p;
      slen = 1;
      prev_len = len;
    }
    p += len;
  }

  r = add_compile_string_length(prev, prev_len, slen, reg);
  rlen += r;
  return rlen;
}