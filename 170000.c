static int deleteCell(Rtree *pRtree, RtreeNode *pNode, int iCell, int iHeight){
  RtreeNode *pParent;
  int rc;

  if( SQLITE_OK!=(rc = fixLeafParent(pRtree, pNode)) ){
    return rc;
  }

  /* Remove the cell from the node. This call just moves bytes around
  ** the in-memory node image, so it cannot fail.
  */
  nodeDeleteCell(pRtree, pNode, iCell);

  /* If the node is not the tree root and now has less than the minimum
  ** number of cells, remove it from the tree. Otherwise, update the
  ** cell in the parent node so that it tightly contains the updated
  ** node.
  */
  pParent = pNode->pParent;
  assert( pParent || pNode->iNode==1 );
  if( pParent ){
    if( NCELL(pNode)<RTREE_MINCELLS(pRtree) ){
      rc = removeNode(pRtree, pNode, iHeight);
    }else{
      rc = fixBoundingBox(pRtree, pNode);
    }
  }

  return rc;
}