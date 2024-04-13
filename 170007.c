static void nodeBlobReset(Rtree *pRtree){
  if( pRtree->pNodeBlob && pRtree->inWrTrans==0 && pRtree->nCursor==0 ){
    sqlite3_blob *pBlob = pRtree->pNodeBlob;
    pRtree->pNodeBlob = 0;
    sqlite3_blob_close(pBlob);
  }
}