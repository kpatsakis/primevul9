_gcry_rngcsprng_initialize (int full)
{
  if (!full)
    initialize_basics ();
  else
    initialize ();
}