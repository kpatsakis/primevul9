static void rtreeRelease(Rtree *pRtree){
  pRtree->nBusy--;
  if( pRtree->nBusy==0 ){
    pRtree->inWrTrans = 0;
    assert( pRtree->nCursor==0 );
    nodeBlobReset(pRtree);
    assert( pRtree->nNodeRef==0 || pRtree->bCorrupt );
    sqlite3_finalize(pRtree->pWriteNode);
    sqlite3_finalize(pRtree->pDeleteNode);
    sqlite3_finalize(pRtree->pReadRowid);
    sqlite3_finalize(pRtree->pWriteRowid);
    sqlite3_finalize(pRtree->pDeleteRowid);
    sqlite3_finalize(pRtree->pReadParent);
    sqlite3_finalize(pRtree->pWriteParent);
    sqlite3_finalize(pRtree->pDeleteParent);
    sqlite3_finalize(pRtree->pWriteAux);
    sqlite3_free(pRtree->zReadAuxSql);
    sqlite3_free(pRtree);
  }
}