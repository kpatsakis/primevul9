random_poll()
{
  rndstats.slowpolls++;
  read_random_source (RANDOM_ORIGIN_SLOWPOLL, POOLSIZE/5, GCRY_STRONG_RANDOM);
}