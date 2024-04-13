is_anychar_infinite_greedy(QuantNode* qn)
{
  if (qn->greedy && IS_INFINITE_REPEAT(qn->upper) &&
      NODE_IS_ANYCHAR(NODE_QUANT_BODY(qn)))
    return 1;
  else
    return 0;
}