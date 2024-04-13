static int nodeInsertCell(
  Rtree *pRtree,                /* The overall R-Tree */
  RtreeNode *pNode,             /* Write new cell into this node */
  RtreeCell *pCell              /* The cell to be inserted */
){
  int nCell;                    /* Current number of cells in pNode */
  int nMaxCell;                 /* Maximum number of cells for pNode */

  nMaxCell = (pRtree->iNodeSize-4)/pRtree->nBytesPerCell;
  nCell = NCELL(pNode);

  assert( nCell<=nMaxCell );
  if( nCell<nMaxCell ){
    nodeOverwriteCell(pRtree, pNode, pCell, nCell);
    writeInt16(&pNode->zData[2], nCell+1);
    pNode->isDirty = 1;
  }

  return (nCell==nMaxCell);
}