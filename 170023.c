static void rtreeSearchPointSwap(RtreeCursor *p, int i, int j){
  RtreeSearchPoint t = p->aPoint[i];
  assert( i<j );
  p->aPoint[i] = p->aPoint[j];
  p->aPoint[j] = t;
  i++; j++;
  if( i<RTREE_CACHE_SZ ){
    if( j>=RTREE_CACHE_SZ ){
      nodeRelease(RTREE_OF_CURSOR(p), p->aNode[i]);
      p->aNode[i] = 0;
    }else{
      RtreeNode *pTemp = p->aNode[i];
      p->aNode[i] = p->aNode[j];
      p->aNode[j] = pTemp;
    }
  }
}