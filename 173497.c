onig_reduce_nested_quantifier(Node* pnode, Node* cnode)
{
  int pnum, cnum;
  QtfrNode *p, *c;

  p = QTFR_(pnode);
  c = QTFR_(cnode);
  pnum = popular_quantifier_num(p);
  cnum = popular_quantifier_num(c);
  if (pnum < 0 || cnum < 0) return ;

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

  NODE_BODY(cnode) = NULL_NODE;
  onig_node_free(cnode);
}