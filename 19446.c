time_diff_ms(struct timeval *t1, struct timeval *t2)
{
    // This handles wrapping of tv_usec correctly without any special case.
    // Example of 2 pairs (tv_sec, tv_usec) with a duration of 5 ms:
    //	   t1 = (1, 998000) t2 = (2, 3000) gives:
    //	   (2 - 1) * 1000 + (3000 - 998000) / 1000 -> 5 ms.
    return (t2->tv_sec - t1->tv_sec) * 1000
	 + (t2->tv_usec - t1->tv_usec) / 1000;
}