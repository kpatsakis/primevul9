quantifier_type_num(QuantNode* q)
{
  if (q->greedy) {
    if (q->lower == 0) {
      if (q->upper == 1) return 0;
      else if (IS_REPEAT_INFINITE(q->upper)) return 1;
    }
    else if (q->lower == 1) {
      if (IS_REPEAT_INFINITE(q->upper)) return 2;
    }
  }
  else {
    if (q->lower == 0) {
      if (q->upper == 1) return 3;
      else if (IS_REPEAT_INFINITE(q->upper)) return 4;
    }
    else if (q->lower == 1) {
      if (IS_REPEAT_INFINITE(q->upper)) return 5;
    }
  }
  return -1;
}