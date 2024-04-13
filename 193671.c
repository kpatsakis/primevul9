is_exclusive(Node* x, Node* y, regex_t* reg)
{
  int i, len;
  OnigCodePoint code;
  UChar *p;
  NodeType ytype;

 retry:
  ytype = NODE_TYPE(y);
  switch (NODE_TYPE(x)) {
  case NODE_CTYPE:
    {
      if (CTYPE_(x)->ctype == CTYPE_ANYCHAR ||
          CTYPE_(y)->ctype == CTYPE_ANYCHAR)
        break;

      switch (ytype) {
      case NODE_CTYPE:
        if (CTYPE_(y)->ctype == CTYPE_(x)->ctype &&
            CTYPE_(y)->not   != CTYPE_(x)->not &&
            CTYPE_(y)->ascii_mode == CTYPE_(x)->ascii_mode)
          return 1;
        else
          return 0;
        break;

      case NODE_CCLASS:
      swap:
        {
          Node* tmp;
          tmp = x; x = y; y = tmp;
          goto retry;
        }
        break;

      case NODE_STRING:
        goto swap;
        break;

      default:
        break;
      }
    }
    break;

  case NODE_CCLASS:
    {
      int range;
      CClassNode* xc = CCLASS_(x);

      switch (ytype) {
      case NODE_CTYPE:
        switch (CTYPE_(y)->ctype) {
        case CTYPE_ANYCHAR:
          return 0;
          break;

        case ONIGENC_CTYPE_WORD:
          if (CTYPE_(y)->not == 0) {
            if (IS_NULL(xc->mbuf) && !IS_NCCLASS_NOT(xc)) {
              range = CTYPE_(y)->ascii_mode != 0 ? 128 : SINGLE_BYTE_SIZE;
              for (i = 0; i < range; i++) {
                if (BITSET_AT(xc->bs, i)) {
                  if (ONIGENC_IS_CODE_WORD(reg->enc, i)) return 0;
                }
              }
              return 1;
            }
            return 0;
          }
          else {
            if (IS_NOT_NULL(xc->mbuf)) return 0;
            if (IS_NCCLASS_NOT(xc)) return 0;

            range = CTYPE_(y)->ascii_mode != 0 ? 128 : SINGLE_BYTE_SIZE;
            for (i = 0; i < range; i++) {
              if (! ONIGENC_IS_CODE_WORD(reg->enc, i)) {
                if (BITSET_AT(xc->bs, i))
                  return 0;
              }
            }
            for (i = range; i < SINGLE_BYTE_SIZE; i++) {
              if (BITSET_AT(xc->bs, i)) return 0;
            }
            return 1;
          }
          break;

        default:
          break;
        }
        break;

      case NODE_CCLASS:
        {
          int v;
          CClassNode* yc = CCLASS_(y);

          for (i = 0; i < SINGLE_BYTE_SIZE; i++) {
            v = BITSET_AT(xc->bs, i);
            if ((v != 0 && !IS_NCCLASS_NOT(xc)) || (v == 0 && IS_NCCLASS_NOT(xc))) {
              v = BITSET_AT(yc->bs, i);
              if ((v != 0 && !IS_NCCLASS_NOT(yc)) ||
                  (v == 0 && IS_NCCLASS_NOT(yc)))
                return 0;
            }
          }
          if ((IS_NULL(xc->mbuf) && !IS_NCCLASS_NOT(xc)) ||
              (IS_NULL(yc->mbuf) && !IS_NCCLASS_NOT(yc)))
            return 1;
          return 0;
        }
        break;

      case NODE_STRING:
        goto swap;
        break;

      default:
        break;
      }
    }
    break;

  case NODE_STRING:
    {
      StrNode* xs = STR_(x);

      if (NODE_STRING_LEN(x) == 0)
        break;

      //c = *(xs->s);
      switch (ytype) {
      case NODE_CTYPE:
        switch (CTYPE_(y)->ctype) {
        case CTYPE_ANYCHAR:
          break;

        case ONIGENC_CTYPE_WORD:
          if (CTYPE_(y)->ascii_mode == 0) {
            if (ONIGENC_IS_MBC_WORD(reg->enc, xs->s, xs->end))
              return CTYPE_(y)->not;
            else
              return !(CTYPE_(y)->not);
          }
          else {
            if (ONIGENC_IS_MBC_WORD_ASCII(reg->enc, xs->s, xs->end))
              return CTYPE_(y)->not;
            else
              return !(CTYPE_(y)->not);
          }
          break;
        default:
          break;
        }
        break;

      case NODE_CCLASS:
        {
          CClassNode* cc = CCLASS_(y);

          code = ONIGENC_MBC_TO_CODE(reg->enc, xs->s,
                                     xs->s + ONIGENC_MBC_MAXLEN(reg->enc));
          return (onig_is_code_in_cc(reg->enc, code, cc) != 0 ? 0 : 1);
        }
        break;

      case NODE_STRING:
        {
          UChar *q;
          StrNode* ys = STR_(y);

          len = NODE_STRING_LEN(x);
          if (len > NODE_STRING_LEN(y)) len = NODE_STRING_LEN(y);
          if (NODE_STRING_IS_AMBIG(x) || NODE_STRING_IS_AMBIG(y)) {
            /* tiny version */
            return 0;
          }
          else {
            for (i = 0, p = ys->s, q = xs->s; i < len; i++, p++, q++) {
              if (*p != *q) return 1;
            }
          }
        }
        break;

      default:
        break;
      }
    }
    break;

  default:
    break;
  }

  return 0;
}