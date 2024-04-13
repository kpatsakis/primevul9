print_indent_tree(FILE* f, Node* node, int indent)
{
  int i;
  NodeType type;
  UChar* p;
  int add = 3;

  Indent(f, indent);
  if (IS_NULL(node)) {
    fprintf(f, "ERROR: null node!!!\n");
    exit(0);
  }

  type = NODE_TYPE(node);
  switch (type) {
  case NODE_LIST:
  case NODE_ALT:
    if (type == NODE_LIST)
      fprintf(f, "<list:%p>\n", node);
    else
      fprintf(f, "<alt:%p>\n", node);

    print_indent_tree(f, NODE_CAR(node), indent + add);
    while (IS_NOT_NULL(node = NODE_CDR(node))) {
      if (NODE_TYPE(node) != type) {
        fprintf(f, "ERROR: list/alt right is not a cons. %d\n", NODE_TYPE(node));
        exit(0);
      }
      print_indent_tree(f, NODE_CAR(node), indent + add);
    }
    break;

  case NODE_STRING:
    {
      char* str;
      char* mode;

      if (NODE_STRING_IS_CRUDE(node))
        mode = "-crude";
      else if (NODE_IS_IGNORECASE(node))
        mode = "-ignorecase";
      else
        mode = "";

      if (STR_(node)->s == STR_(node)->end)
        str = "empty-string";
      else
        str = "string";

      fprintf(f, "<%s%s:%p>", str, mode, node);
      for (p = STR_(node)->s; p < STR_(node)->end; p++) {
        if (*p >= 0x20 && *p < 0x7f)
          fputc(*p, f);
        else {
          fprintf(f, " 0x%02x", *p);
        }
      }
    }
    break;

  case NODE_CCLASS:
#define CCLASS_MBUF_MAX_OUTPUT_NUM   10

    fprintf(f, "<cclass:%p>", node);
    if (IS_NCCLASS_NOT(CCLASS_(node))) fputs(" not", f);
    if (CCLASS_(node)->mbuf) {
      BBuf* bbuf = CCLASS_(node)->mbuf;
      fprintf(f, " mbuf(%u) ", bbuf->used);
      for (i = 0; i < bbuf->used && i < CCLASS_MBUF_MAX_OUTPUT_NUM; i++) {
        if (i > 0) fprintf(f, ",");
        fprintf(f, "%0x", bbuf->p[i]);
      }
      if (i < bbuf->used) fprintf(f, "...");
    }
    break;

  case NODE_CTYPE:
    fprintf(f, "<ctype:%p> ", node);
    switch (CTYPE_(node)->ctype) {
    case CTYPE_ANYCHAR:
      fprintf(f, "anychar");
      break;

    case ONIGENC_CTYPE_WORD:
      if (CTYPE_(node)->not != 0)
        fputs("not word", f);
      else
        fputs("word",     f);

      if (CTYPE_(node)->ascii_mode != 0)
        fputs(" (ascii)", f);

      break;

    default:
      fprintf(f, "ERROR: undefined ctype.\n");
      exit(0);
    }
    break;

  case NODE_ANCHOR:
    fprintf(f, "<anchor:%p> ", node);
    switch (ANCHOR_(node)->type) {
    case ANCR_BEGIN_BUF:        fputs("begin buf",      f); break;
    case ANCR_END_BUF:          fputs("end buf",        f); break;
    case ANCR_BEGIN_LINE:       fputs("begin line",     f); break;
    case ANCR_END_LINE:         fputs("end line",       f); break;
    case ANCR_SEMI_END_BUF:     fputs("semi end buf",   f); break;
    case ANCR_BEGIN_POSITION:   fputs("begin position", f); break;

    case ANCR_WORD_BOUNDARY:    fputs("word boundary",     f); break;
    case ANCR_NO_WORD_BOUNDARY: fputs("not word boundary", f); break;
#ifdef USE_WORD_BEGIN_END
    case ANCR_WORD_BEGIN:       fputs("word begin", f);     break;
    case ANCR_WORD_END:         fputs("word end", f);       break;
#endif
    case ANCR_TEXT_SEGMENT_BOUNDARY:
      fputs("text-segment boundary", f); break;
    case ANCR_NO_TEXT_SEGMENT_BOUNDARY:
      fputs("no text-segment boundary", f); break;
    case ANCR_PREC_READ:
      fprintf(f, "prec read\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;
    case ANCR_PREC_READ_NOT:
      fprintf(f, "prec read not\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;
    case ANCR_LOOK_BEHIND:
      fprintf(f, "look behind\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;
    case ANCR_LOOK_BEHIND_NOT:
      fprintf(f, "look behind not\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);
      break;

    default:
      fprintf(f, "ERROR: undefined anchor type.\n");
      break;
    }
    break;

  case NODE_BACKREF:
    {
      int* p;
      BackRefNode* br = BACKREF_(node);
      p = BACKREFS_P(br);
      fprintf(f, "<backref%s:%p>", NODE_IS_CHECKER(node) ? "-checker" : "", node);
      for (i = 0; i < br->back_num; i++) {
        if (i > 0) fputs(", ", f);
        fprintf(f, "%d", p[i]);
      }
#ifdef USE_BACKREF_WITH_LEVEL
      if (NODE_IS_NEST_LEVEL(node)) {
        fprintf(f, ", level: %d", br->nest_level);
      }
#endif
    }
    break;

#ifdef USE_CALL
  case NODE_CALL:
    {
      CallNode* cn = CALL_(node);
      fprintf(f, "<call:%p>", node);
      fprintf(f, " num: %d, name", cn->called_gnum);
      p_string(f, cn->name_end - cn->name, cn->name);
    }
    break;
#endif

  case NODE_QUANT:
    fprintf(f, "<quantifier:%p>{%d,%d}%s%s\n", node,
            QUANT_(node)->lower, QUANT_(node)->upper,
            (QUANT_(node)->greedy ? "" : "?"),
            QUANT_(node)->include_referred == 0 ? "" : " referred");
    print_indent_tree(f, NODE_BODY(node), indent + add);
    break;

  case NODE_BAG:
    fprintf(f, "<bag:%p> ", node);
    if (BAG_(node)->type == BAG_IF_ELSE) {
      Node* Then;
      Node* Else;
      BagNode* bn;

      bn = BAG_(node);
      fprintf(f, "if-else\n");
      print_indent_tree(f, NODE_BODY(node), indent + add);

      Then = bn->te.Then;
      Else = bn->te.Else;
      if (IS_NULL(Then)) {
        Indent(f, indent + add);
        fprintf(f, "THEN empty\n");
      }
      else
        print_indent_tree(f, Then, indent + add);

      if (IS_NULL(Else)) {
        Indent(f, indent + add);
        fprintf(f, "ELSE empty\n");
      }
      else
        print_indent_tree(f, Else, indent + add);

      break;
    }

    switch (BAG_(node)->type) {
    case BAG_OPTION:
      fprintf(f, "option:%d", BAG_(node)->o.options);
      break;
    case BAG_MEMORY:
      fprintf(f, "memory:%d", BAG_(node)->m.regnum);
      if (NODE_IS_CALLED(node))
        fprintf(f, ", called");
      else if (NODE_IS_REFERENCED(node))
        fprintf(f, ", referenced");
      if (NODE_IS_FIXED_ADDR(node))
        fprintf(f, ", fixed-addr");
      break;
    case BAG_STOP_BACKTRACK:
      fprintf(f, "stop-bt");
      break;
    default:
      break;
    }
    fprintf(f, "\n");
    print_indent_tree(f, NODE_BODY(node), indent + add);
    break;

  case NODE_GIMMICK:
    fprintf(f, "<gimmick:%p> ", node);
    switch (GIMMICK_(node)->type) {
    case GIMMICK_FAIL:
      fprintf(f, "fail");
      break;
    case GIMMICK_SAVE:
      fprintf(f, "save:%d:%d", GIMMICK_(node)->detail_type, GIMMICK_(node)->id);
      break;
    case GIMMICK_UPDATE_VAR:
      fprintf(f, "update_var:%d:%d", GIMMICK_(node)->detail_type, GIMMICK_(node)->id);
      break;
#ifdef USE_CALLOUT
    case GIMMICK_CALLOUT:
      switch (GIMMICK_(node)->detail_type) {
      case ONIG_CALLOUT_OF_CONTENTS:
        fprintf(f, "callout:contents:%d", GIMMICK_(node)->num);
        break;
      case ONIG_CALLOUT_OF_NAME:
        fprintf(f, "callout:name:%d:%d", GIMMICK_(node)->id, GIMMICK_(node)->num);
        break;
      }
#endif
    }
    break;

  default:
    fprintf(f, "print_indent_tree: undefined node type %d\n", NODE_TYPE(node));
    break;
  }

  if (type != NODE_LIST && type != NODE_ALT && type != NODE_QUANT &&
      type != NODE_BAG)
    fprintf(f, "\n");
  fflush(f);
}