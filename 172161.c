onig_reduce_nested_quantifier(Node* pnode, Node* cnode)
{
  int pnum, cnum;
  QuantNode *p, *c;

  p = QUANT_(pnode);
  c = QUANT_(cnode);
  pnum = quantifier_type_num(p);
  cnum = quantifier_type_num(c);
  if (pnum < 0 || cnum < 0) {
    if ((p->lower == p->upper) && ! IS_REPEAT_INFINITE(p->upper)) {
      if ((c->lower == c->upper) && ! IS_REPEAT_INFINITE(c->upper)) {
        int n = positive_int_multiply(p->lower, c->lower);
        if (n >= 0) {
          p->lower = p->upper = n;
          NODE_BODY(pnode) = NODE_BODY(cnode);
          goto remove_cnode;
        }
      }
    }

    return ;
  }

  switch(ReduceTypeTable[cnum][pnum]) {
  case RQ_DEL:
    *pnode = *cnode;
    break;
  case RQ_A:
    NODE_BODY(pnode) = NODE_BODY(cnode);
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 1;
    break;
  case RQ_AQ:
    NODE_BODY(pnode) = NODE_BODY(cnode);
    p->lower  = 0;  p->upper = REPEAT_INFINITE;  p->greedy = 0;
    break;
  case RQ_QQ:
    NODE_BODY(pnode) = NODE_BODY(cnode);
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    break;
  case RQ_P_QQ:
    NODE_BODY(pnode) = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 0;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 1;
    return ;
    break;
  case RQ_PQ_Q:
    NODE_BODY(pnode) = cnode;
    p->lower  = 0;  p->upper = 1;  p->greedy = 1;
    c->lower  = 1;  c->upper = REPEAT_INFINITE;  c->greedy = 0;
    return ;
    break;
  case RQ_ASIS:
    NODE_BODY(pnode) = cnode;
    return ;
    break;
  }

 remove_cnode:
  NODE_BODY(cnode) = NULL_NODE;
  onig_node_free(cnode);
}