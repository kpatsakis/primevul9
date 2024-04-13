compile_tree(Node* node, regex_t* reg, ScanEnv* env)
{
  int n, len, pos, r = 0;

  switch (NODE_TYPE(node)) {
  case NODE_LIST:
    do {
      r = compile_tree(NODE_CAR(node), reg, env);
    } while (r == 0 && IS_NOT_NULL(node = NODE_CDR(node)));
    break;

  case NODE_ALT:
    {
      Node* x = node;
      len = 0;
      do {
        len += compile_length_tree(NODE_CAR(x), reg);
        if (IS_NOT_NULL(NODE_CDR(x))) {
          len += OPSIZE_PUSH + OPSIZE_JUMP;
        }
      } while (IS_NOT_NULL(x = NODE_CDR(x)));
      pos = COP_CURR_OFFSET(reg) + 1 + len;  /* goal position */

      do {
        len = compile_length_tree(NODE_CAR(node), reg);
        if (IS_NOT_NULL(NODE_CDR(node))) {
          enum OpCode push = NODE_IS_SUPER(node) ? OP_PUSH_SUPER : OP_PUSH;
          r = add_op(reg, push);
          if (r != 0) break;
          COP(reg)->push.addr = SIZE_INC + len + OPSIZE_JUMP;
        }
        r = compile_tree(NODE_CAR(node), reg, env);
        if (r != 0) break;
        if (IS_NOT_NULL(NODE_CDR(node))) {
          len = pos - (COP_CURR_OFFSET(reg) + 1);
          r = add_op(reg, OP_JUMP);
          if (r != 0) break;
          COP(reg)->jump.addr = len;
        }
      } while (IS_NOT_NULL(node = NODE_CDR(node)));
    }
    break;

  case NODE_STRING:
    if (NODE_STRING_IS_CRUDE(node))
      r = compile_string_crude_node(STR_(node), reg);
    else
      r = compile_string_node(node, reg);
    break;

  case NODE_CCLASS:
    r = compile_cclass_node(CCLASS_(node), reg);
    break;

  case NODE_CTYPE:
    {
      int op;

      switch (CTYPE_(node)->ctype) {
      case CTYPE_ANYCHAR:
        r = add_op(reg, NODE_IS_MULTILINE(node) ? OP_ANYCHAR_ML : OP_ANYCHAR);
        break;

      case ONIGENC_CTYPE_WORD:
        if (CTYPE_(node)->ascii_mode == 0) {
          op = CTYPE_(node)->not != 0 ? OP_NO_WORD : OP_WORD;
        }
        else {
          op = CTYPE_(node)->not != 0 ? OP_NO_WORD_ASCII : OP_WORD_ASCII;
        }
        r = add_op(reg, op);
        break;

      default:
        return ONIGERR_TYPE_BUG;
        break;
      }
    }
    break;

  case NODE_BACKREF:
    {
      BackRefNode* br = BACKREF_(node);

      if (NODE_IS_CHECKER(node)) {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          r = add_op(reg, OP_BACKREF_CHECK_WITH_LEVEL);
          if (r != 0) return r;
          COP(reg)->backref_general.nest_level = br->nest_level;
        }
        else
#endif
          {
            r = add_op(reg, OP_BACKREF_CHECK);
            if (r != 0) return r;
          }
        goto add_bacref_mems;
      }
      else {
#ifdef USE_BACKREF_WITH_LEVEL
        if (NODE_IS_NEST_LEVEL(node)) {
          if (NODE_IS_IGNORECASE(node))
            r = add_op(reg, OP_BACKREF_WITH_LEVEL_IC);
          else
            r = add_op(reg, OP_BACKREF_WITH_LEVEL);

          if (r != 0) return r;
          COP(reg)->backref_general.nest_level = br->nest_level;
          goto add_bacref_mems;
        }
        else
#endif
        if (br->back_num == 1) {
          n = br->back_static[0];
          if (NODE_IS_IGNORECASE(node)) {
            r = add_op(reg, OP_BACKREF_N_IC);
            if (r != 0) return r;
            COP(reg)->backref_n.n1 = n;
          }
          else {
            switch (n) {
            case 1:  r = add_op(reg, OP_BACKREF1); break;
            case 2:  r = add_op(reg, OP_BACKREF2); break;
            default:
              r = add_op(reg, OP_BACKREF_N);
              if (r != 0) return r;
              COP(reg)->backref_n.n1 = n;
              break;
            }
          }
        }
        else {
          int num;
          int* p;

          r = add_op(reg, NODE_IS_IGNORECASE(node) ?
                     OP_BACKREF_MULTI_IC : OP_BACKREF_MULTI);
          if (r != 0) return r;

        add_bacref_mems:
          num = br->back_num;
          COP(reg)->backref_general.num = num;
          if (num == 1) {
            COP(reg)->backref_general.n1 = br->back_static[0];
          }
          else {
            int i, j;
            MemNumType* ns;

            ns = xmalloc(sizeof(MemNumType) * num);
            CHECK_NULL_RETURN_MEMERR(ns);
            COP(reg)->backref_general.ns = ns;
            p = BACKREFS_P(br);
            for (i = num - 1, j = 0; i >= 0; i--, j++) {
              ns[j] = p[i];
            }
          }
        }
      }
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    r = compile_call(CALL_(node), reg, env);
    break;
#endif

  case NODE_QUANT:
    r = compile_quantifier_node(QUANT_(node), reg, env);
    break;

  case NODE_BAG:
    r = compile_bag_node(BAG_(node), reg, env);
    break;

  case NODE_ANCHOR:
    r = compile_anchor_node(ANCHOR_(node), reg, env);
    break;

  case NODE_GIMMICK:
    r = compile_gimmick_node(GIMMICK_(node), reg);
    break;

  default:
#ifdef ONIG_DEBUG
    fprintf(DBGFP, "compile_tree: undefined node type %d\n", NODE_TYPE(node));
#endif
    break;
  }

  return r;
}