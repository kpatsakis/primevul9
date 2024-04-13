countFrequencies (Int64 freq[HUF_ENCSIZE],
		  const unsigned short data[/*n*/],
		  int n)
#endif
{
    for (int i = 0; i < HUF_ENCSIZE; ++i)
	freq[i] = 0;

    for (int i = 0; i < n; ++i)
	++freq[data[i]];
}