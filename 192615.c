S_edit_distance(const UV* src,
                const UV* tgt,
                const STRLEN x,             /* length of src[] */
                const STRLEN y,             /* length of tgt[] */
                const SSize_t maxDistance
)
{
    item *head = NULL;
    UV swapCount,swapScore,targetCharCount,i,j;
    UV *scores;
    UV score_ceil = x + y;

    PERL_ARGS_ASSERT_EDIT_DISTANCE;

    /* intialize matrix start values */
    Newxz(scores, ( (x + 2) * (y + 2)), UV);
    scores[0] = score_ceil;
    scores[1 * (y + 2) + 0] = score_ceil;
    scores[0 * (y + 2) + 1] = score_ceil;
    scores[1 * (y + 2) + 1] = 0;
    head = uniquePush(uniquePush(head,src[0]),tgt[0]);

    /* work loops    */
    /* i = src index */
    /* j = tgt index */
    for (i=1;i<=x;i++) {
        if (i < x)
            head = uniquePush(head,src[i]);
        scores[(i+1) * (y + 2) + 1] = i;
        scores[(i+1) * (y + 2) + 0] = score_ceil;
        swapCount = 0;

        for (j=1;j<=y;j++) {
            if (i == 1) {
                if(j < y)
                head = uniquePush(head,tgt[j]);
                scores[1 * (y + 2) + (j + 1)] = j;
                scores[0 * (y + 2) + (j + 1)] = score_ceil;
            }

            targetCharCount = find(head,tgt[j-1])->value;
            swapScore = scores[targetCharCount * (y + 2) + swapCount] + i - targetCharCount - 1 + j - swapCount;

            if (src[i-1] != tgt[j-1]){
                scores[(i+1) * (y + 2) + (j + 1)] = MIN(swapScore,(MIN(scores[i * (y + 2) + j], MIN(scores[(i+1) * (y + 2) + j], scores[i * (y + 2) + (j + 1)])) + 1));
            }
            else {
                swapCount = j;
                scores[(i+1) * (y + 2) + (j + 1)] = MIN(scores[i * (y + 2) + j], swapScore);
            }
        }

        find(head,src[i-1])->value = i;
    }

    {
        IV score = scores[(x+1) * (y + 2) + (y + 1)];
        dict_free(head);
        Safefree(scores);
        return (maxDistance != 0 && maxDistance < score)?(-1):score;
    }
}