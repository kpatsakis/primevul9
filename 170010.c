static void nodeGetCell(
  Rtree *pRtree,               /* The overall R-Tree */
  RtreeNode *pNode,            /* The node containing the cell to be read */
  int iCell,                   /* Index of the cell within the node */
  RtreeCell *pCell             /* OUT: Write the cell contents here */
){
  u8 *pData;
  RtreeCoord *pCoord;
  int ii = 0;
  pCell->iRowid = nodeGetRowid(pRtree, pNode, iCell);
  pData = pNode->zData + (12 + pRtree->nBytesPerCell*iCell);
  pCoord = pCell->aCoord;
  do{
    readCoord(pData, &pCoord[ii]);
    readCoord(pData+4, &pCoord[ii+1]);
    pData += 8;
    ii += 2;
  }while( ii<pRtree->nDim2 );
}