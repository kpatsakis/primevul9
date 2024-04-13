set_quantifier(Node* qnode, Node* target, int group, ScanEnv* env)
{
  QuantNode* qn;

  qn = QUANT_(qnode);
  if (qn->lower == 1 && qn->upper == 1)
    return 1;

  switch (NODE_TYPE(target)) {
  case NODE_STRING:
    if (! group) {
      StrNode* sn = STR_(target);
      if (str_node_can_be_split(sn, env->enc)) {
        Node* n = str_node_split_last_char(sn, env->enc);
        if (IS_NOT_NULL(n)) {
          NODE_BODY(qnode) = n;
          return 2;
        }
      }
    }
    break;

  case NODE_QUANT:
    { /* check redundant double repeat. */
      /* verbose warn (?:.?)? etc... but not warn (.?)? etc... */
      QuantNode* qnt   = QUANT_(target);
      int nestq_num   = quantifier_type_num(qn);
      int targetq_num = quantifier_type_num(qnt);

#ifdef USE_WARNING_REDUNDANT_NESTED_REPEAT_OPERATOR
      if (targetq_num >= 0 && nestq_num >= 0 &&
          IS_SYNTAX_BV(env->syntax, ONIG_SYN_WARN_REDUNDANT_NESTED_REPEAT)) {
        UChar buf[WARN_BUFSIZE];

        switch(ReduceTypeTable[targetq_num][nestq_num]) {
        case RQ_ASIS:
          break;

        case RQ_DEL:
          if (onig_verb_warn != onig_null_warn) {
            onig_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
                                  env->pattern, env->pattern_end,
                                  (UChar* )"redundant nested repeat operator");
            (*onig_verb_warn)((char* )buf);
          }
          goto warn_exit;
          break;

        default:
          if (onig_verb_warn != onig_null_warn) {
            onig_snprintf_with_pattern(buf, WARN_BUFSIZE, env->enc,
                                       env->pattern, env->pattern_end,
            (UChar* )"nested repeat operator %s and %s was replaced with '%s'",
            PopularQStr[targetq_num], PopularQStr[nestq_num],
            ReduceQStr[ReduceTypeTable[targetq_num][nestq_num]]);
            (*onig_verb_warn)((char* )buf);
          }
          goto warn_exit;
          break;
        }
      }

    warn_exit:
#endif
      if (targetq_num >= 0 && nestq_num < 0) {
        if (targetq_num == 1 || targetq_num == 2) { /* * or + */
          /* (?:a*){n,m}, (?:a+){n,m} => (?:a*){n,n}, (?:a+){n,n} */
          if (! IS_REPEAT_INFINITE(qn->upper) && qn->upper > 1 && qn->greedy) {
            qn->upper = (qn->lower == 0 ? 1 : qn->lower);
          }
        }
      }
      else {
        NODE_BODY(qnode) = target;
        onig_reduce_nested_quantifier(qnode, target);
        goto q_exit;
      }
    }
    break;

  default:
    break;
  }

  NODE_BODY(qnode) = target;
 q_exit:
  return 0;
}