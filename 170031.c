static int nodeInParentChain(const RtreeNode *pNode, const RtreeNode *pParent){
  do{
    if( pNode==pParent ) return 1;
    pParent = pParent->pParent;
  }while( pParent );
  return 0;
}