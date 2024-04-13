static void freeCursorConstraints(RtreeCursor *pCsr){
  if( pCsr->aConstraint ){
    int i;                        /* Used to iterate through constraint array */
    for(i=0; i<pCsr->nConstraint; i++){
      sqlite3_rtree_query_info *pInfo = pCsr->aConstraint[i].pInfo;
      if( pInfo ){
        if( pInfo->xDelUser ) pInfo->xDelUser(pInfo->pUser);
        sqlite3_free(pInfo);
      }
    }
    sqlite3_free(pCsr->aConstraint);
    pCsr->aConstraint = 0;
  }
}