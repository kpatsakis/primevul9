static void SortByDistance(
  int *aIdx, 
  int nIdx, 
  RtreeDValue *aDistance, 
  int *aSpare
){
  if( nIdx>1 ){
    int iLeft = 0;
    int iRight = 0;

    int nLeft = nIdx/2;
    int nRight = nIdx-nLeft;
    int *aLeft = aIdx;
    int *aRight = &aIdx[nLeft];

    SortByDistance(aLeft, nLeft, aDistance, aSpare);
    SortByDistance(aRight, nRight, aDistance, aSpare);

    memcpy(aSpare, aLeft, sizeof(int)*nLeft);
    aLeft = aSpare;

    while( iLeft<nLeft || iRight<nRight ){
      if( iLeft==nLeft ){
        aIdx[iLeft+iRight] = aRight[iRight];
        iRight++;
      }else if( iRight==nRight ){
        aIdx[iLeft+iRight] = aLeft[iLeft];
        iLeft++;
      }else{
        RtreeDValue fLeft = aDistance[aLeft[iLeft]];
        RtreeDValue fRight = aDistance[aRight[iRight]];
        if( fLeft<fRight ){
          aIdx[iLeft+iRight] = aLeft[iLeft];
          iLeft++;
        }else{
          aIdx[iLeft+iRight] = aRight[iRight];
          iRight++;
        }
      }
    }

#if 0
    /* Check that the sort worked */
    {
      int jj;
      for(jj=1; jj<nIdx; jj++){
        RtreeDValue left = aDistance[aIdx[jj-1]];
        RtreeDValue right = aDistance[aIdx[jj]];
        assert( left<=right );
      }
    }
#endif
  }
}