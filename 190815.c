RsaCheckPrime(
	      bigNum           prime,
	      UINT32           exponent,
	      RAND_STATE      *rand
	      )
{
#if !RSA_KEY_SIEVE
    TPM_RC          retVal = TPM_RC_SUCCESS;
    UINT32          modE = BnModWord(prime, exponent);
    NOT_REFERENCED(rand);
    if(modE == 0)
	// evenly divisible so add two keeping the number odd
	BnAddWord(prime, prime, 2);
    // want 0 != (p - 1) mod e
    // which is 1 != p mod e
    else if(modE == 1)
	// subtract 2 keeping number odd and insuring that
	// 0 != (p - 1) mod e
	BnSubWord(prime, prime, 2);
    if(BnIsProbablyPrime(prime, rand) == 0)
	ERROR_RETURN(TPM_RC_VALUE);
 Exit:
    return retVal;
#else
    return PrimeSelectWithSieve(prime, exponent, rand);
#endif
}