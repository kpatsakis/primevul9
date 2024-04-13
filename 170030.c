static int rtreeSearchPointCompare(
  const RtreeSearchPoint *pA,
  const RtreeSearchPoint *pB
){
  if( pA->rScore<pB->rScore ) return -1;
  if( pA->rScore>pB->rScore ) return +1;
  if( pA->iLevel<pB->iLevel ) return -1;
  if( pA->iLevel>pB->iLevel ) return +1;
  return 0;
}