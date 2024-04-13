read_random_source (enum random_origins origin, size_t length, int level)
{
  if ( !slow_gather_fnc )
    log_fatal ("Slow entropy gathering module not yet initialized\n");

  if (slow_gather_fnc (add_randomness, origin, length, level) < 0)
    log_fatal ("No way to gather entropy for the RNG\n");
}