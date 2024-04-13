static int ChooseLeaf(
  Rtree *pRtree,               /* Rtree table */
  RtreeCell *pCell,            /* Cell to insert into rtree */
  int iHeight,                 /* Height of sub-tree rooted at pCell */
  RtreeNode **ppLeaf           /* OUT: Selected leaf page */
){
  int rc;
  int ii;
  RtreeNode *pNode = 0;
  rc = nodeAcquire(pRtree, 1, 0, &pNode);

  for(ii=0; rc==SQLITE_OK && ii<(pRtree->iDepth-iHeight); ii++){
    int iCell;
    sqlite3_int64 iBest = 0;

    RtreeDValue fMinGrowth = RTREE_ZERO;
    RtreeDValue fMinArea = RTREE_ZERO;

    int nCell = NCELL(pNode);
    RtreeCell cell;
    RtreeNode *pChild;

    RtreeCell *aCell = 0;

    /* Select the child node which will be enlarged the least if pCell
    ** is inserted into it. Resolve ties by choosing the entry with
    ** the smallest area.
    */
    for(iCell=0; iCell<nCell; iCell++){
      int bBest = 0;
      RtreeDValue growth;
      RtreeDValue area;
      nodeGetCell(pRtree, pNode, iCell, &cell);
      growth = cellGrowth(pRtree, &cell, pCell);
      area = cellArea(pRtree, &cell);
      if( iCell==0||growth<fMinGrowth||(growth==fMinGrowth && area<fMinArea) ){
        bBest = 1;
      }
      if( bBest ){
        fMinGrowth = growth;
        fMinArea = area;
        iBest = cell.iRowid;
      }
    }

    sqlite3_free(aCell);
    rc = nodeAcquire(pRtree, iBest, pNode, &pChild);
    nodeRelease(pRtree, pNode);
    pNode = pChild;
  }

  *ppLeaf = pNode;
  return rc;
}