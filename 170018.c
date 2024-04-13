static int fixLeafParent(Rtree *pRtree, RtreeNode *pLeaf){
  int rc = SQLITE_OK;
  RtreeNode *pChild = pLeaf;
  while( rc==SQLITE_OK && pChild->iNode!=1 && pChild->pParent==0 ){
    int rc2 = SQLITE_OK;          /* sqlite3_reset() return code */
    sqlite3_bind_int64(pRtree->pReadParent, 1, pChild->iNode);
    rc = sqlite3_step(pRtree->pReadParent);
    if( rc==SQLITE_ROW ){
      RtreeNode *pTest;           /* Used to test for reference loops */
      i64 iNode;                  /* Node number of parent node */

      /* Before setting pChild->pParent, test that we are not creating a
      ** loop of references (as we would if, say, pChild==pParent). We don't
      ** want to do this as it leads to a memory leak when trying to delete
      ** the referenced counted node structures.
      */
      iNode = sqlite3_column_int64(pRtree->pReadParent, 0);
      for(pTest=pLeaf; pTest && pTest->iNode!=iNode; pTest=pTest->pParent);
      if( !pTest ){
        rc2 = nodeAcquire(pRtree, iNode, 0, &pChild->pParent);
      }
    }
    rc = sqlite3_reset(pRtree->pReadParent);
    if( rc==SQLITE_OK ) rc = rc2;
    if( rc==SQLITE_OK && !pChild->pParent ){
      RTREE_IS_CORRUPT(pRtree);
      rc = SQLITE_CORRUPT_VTAB;
    }
    pChild = pChild->pParent;
  }
  return rc;
}