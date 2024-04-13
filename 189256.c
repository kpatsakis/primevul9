void LibRaw::subtract_black()
{

#define BAYERC(row,col,c) imgdata.image[((row) >> IO.shrink)*S.iwidth + ((col) >> IO.shrink)][c] 

    if(C.ph1_black)
        {
            // Phase One compressed format
            int row,col,val,cc;
            for(row=0;row<S.height;row++)
                for(col=0;col<S.width;col++)
                    {
                        cc=FC(row,col);
                        val = BAYERC(row,col,cc) 
                            - C.phase_one_data.t_black 
                            + C.ph1_black[row+S.top_margin][(col + S.left_margin) 
                                                                                >=C.phase_one_data.split_col];
                        if(val<0) val = 0;
                        BAYERC(row,col,cc) = val;
                    }
            C.maximum -= C.black;
            phase_one_correct();
            // recalculate channel maximum
            ZERO(C.channel_maximum);
            for(row=0;row<S.height;row++)
                for(col=0;col<S.width;col++)
                    {
                        cc=FC(row,col);
                        val = BAYERC(row,col,cc);
                        if(C.channel_maximum[cc] > val) C.channel_maximum[cc] = val;
                    }
            // clear P1 black level data
            imgdata.color.phase_one_data.t_black = 0;
            C.ph1_black = 0;
            ZERO(C.cblack);
            C.black = 0;
        }
    else if((C.black || C.cblack[0] || C.cblack[1] || C.cblack[2] || C.cblack[3]))
        {
            int cblk[4],i,row,col,val,cc;
            for(i=0;i<4;i++)
                cblk[i] = C.cblack[i]+C.black;
            ZERO(C.channel_maximum);

            for(row=0;row<S.height;row++)
                for(col=0;col<S.width;col++)
                    {
                        cc=COLOR(row,col);
                        val = BAYERC(row,col,cc);
                        if(val > cblk[cc])
                            val -= cblk[cc];
                        else
                            val = 0;
                        if(C.channel_maximum[cc] < val) C.channel_maximum[cc] = val;
                        BAYERC(row,col,cc) = val;
                    }
            C.maximum -= C.black;
            ZERO(C.cblack);
            C.black = 0;
        }
    else
        {
            // only calculate channel maximum;
            int row,col,cc,val;
            ZERO(C.channel_maximum);
            for(row=0;row<S.height;row++)
                for(col=0;col<S.width;col++)
                    for(cc = 0; cc< 4; cc++)
                        {
                            int val = BAYERC(row,col,cc);
                            if(C.channel_maximum[cc] < val) C.channel_maximum[cc] = val;
                        }
            
        }
}