_gcry_rngcsprng_dump_stats (void)
{
  /* In theory we would need to lock the stats here.  However this
     function is usually called during cleanup and then we _might_ run
     into problems.  */

  log_info ("random usage: poolsize=%d mixed=%lu polls=%lu/%lu added=%lu/%lu\n"
	    "              outmix=%lu getlvl1=%lu/%lu getlvl2=%lu/%lu%s\n",
            POOLSIZE, rndstats.mixrnd, rndstats.slowpolls, rndstats.fastpolls,
            rndstats.naddbytes, rndstats.addbytes,
            rndstats.mixkey, rndstats.ngetbytes1, rndstats.getbytes1,
            rndstats.ngetbytes2, rndstats.getbytes2,
            _gcry_rndhw_failed_p()? " (hwrng failed)":"");
}