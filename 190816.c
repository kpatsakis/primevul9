MillerRabin(
	    bigNum           bnW,
	    RAND_STATE      *rand
	    )
{
    BN_MAX(bnWm1);
    BN_PRIME(bnM);
    BN_PRIME(bnB);
    BN_PRIME(bnZ);
    BOOL             ret = FALSE;   // Assumed composite for easy exit
    unsigned int     a;
    unsigned int     j;
    int              wLen;
    int              i;
    int              iterations = MillerRabinRounds(BnSizeInBits(bnW));
    //
    INSTRUMENT_INC(MillerRabinTrials[PrimeIndex]);
    pAssert(bnW->size > 1);
    // Let a be the largest integer such that 2^a divides w1.
    BnSubWord(bnWm1, bnW, 1);
    pAssert(bnWm1->size != 0);
    // Since w is odd (w-1) is even so start at bit number 1 rather than 0
    // Get the number of bits in bnWm1 so that it doesn't have to be recomputed
    // on each iteration.
    i = bnWm1->size * RADIX_BITS;
    // Now find the largest power of 2 that divides w1
    for(a = 1;
	(a < (bnWm1->size * RADIX_BITS)) &&
	    (BnTestBit(bnWm1, a) == 0);
	a++);
    // 2. m = (w1) / 2^a
    BnShiftRight(bnM, bnWm1, a);
    // 3. wlen = len (w).
    wLen = BnSizeInBits(bnW);
    // 4. For i = 1 to iterations do
    for(i = 0; i < iterations; i++)
	{
	    // 4.1 Obtain a string b of wlen bits from an RBG.
	    // Ensure that 1 < b < w1.
	    do
		{
		    BnGetRandomBits(bnB, wLen, rand);
		    // 4.2 If ((b <= 1) or (b >= w1)), then go to step 4.1.
		} while((BnUnsignedCmpWord(bnB, 1) <= 0)
			|| (BnUnsignedCmp(bnB, bnWm1) >= 0));
	    // 4.3 z = b^m mod w.
	    // if ModExp fails, then say this is not
	    // prime and bail out.
	    BnModExp(bnZ, bnB, bnM, bnW);
	    // 4.4 If ((z == 1) or (z = w == 1)), then go to step 4.7.
	    if((BnUnsignedCmpWord(bnZ, 1) == 0)
	       || (BnUnsignedCmp(bnZ, bnWm1) == 0))
		goto step4point7;
	    // 4.5 For j = 1 to a  1 do.
	    for(j = 1; j < a; j++)
		{
		    // 4.5.1 z = z^2 mod w.
		    BnModMult(bnZ, bnZ, bnZ, bnW);
		    // 4.5.2 If (z = w1), then go to step 4.7.
		    if(BnUnsignedCmp(bnZ, bnWm1) == 0)
			goto step4point7;
		    // 4.5.3 If (z = 1), then go to step 4.6.
		    if(BnEqualWord(bnZ, 1))
			goto step4point6;
		}
	    // 4.6 Return COMPOSITE.
	step4point6:
	    INSTRUMENT_INC(failedAtIteration[i]);
	    goto end;
	    // 4.7 Continue. Comment: Increment i for the do-loop in step 4.
	step4point7:
	    continue;
	}
    // 5. Return PROBABLY PRIME
    ret = TRUE;
 end:
    return ret;
}