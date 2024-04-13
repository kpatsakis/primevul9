void LibRaw::adjust_bl()
{

   if (O.user_black >= 0) 
     C.black = O.user_black;
   for(int i=0; i<4; i++)
     if(O.user_cblack[i]>-1000000)
       C.cblack[i] = O.user_cblack[i];

  // remove common part from C.cblack[]
  int i = C.cblack[3];
  int c;
  for(c=0;c<3;c++) if (i > C.cblack[c]) i = C.cblack[c];
  for(c=0;c<4;c++) C.cblack[c] -= i;
  C.black += i;
  for(c=0;c<4;c++) C.cblack[c] += C.black;
}