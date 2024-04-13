void TDStretch::overlapStereo(short *poutput, const short *input) const
{
    int i;
    short temp;
    int cnt2;

    for (i = 0; i < overlapLength ; i ++) 
    {
        temp = (short)(overlapLength - i);
        cnt2 = 2 * i;
        poutput[cnt2] = (input[cnt2] * i + pMidBuffer[cnt2] * temp )  / overlapLength;
        poutput[cnt2 + 1] = (input[cnt2 + 1] * i + pMidBuffer[cnt2 + 1] * temp ) / overlapLength;
    }
}