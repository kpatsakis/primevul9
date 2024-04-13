int gg_libgadu_check_feature(gg_libgadu_feature_t feature)
{
	switch (feature)
	{
	case GG_LIBGADU_FEATURE_SSL:
#if defined(GG_CONFIG_HAVE_OPENSSL) || defined(GG_CONFIG_HAVE_GNUTLS)
		return 1;
#else
		return 0;
#endif

	case GG_LIBGADU_FEATURE_PTHREAD:
#ifdef GG_CONFIG_HAVE_PTHREAD
		return 1;
#else
		return 0;
#endif

	case GG_LIBGADU_FEATURE_USERLIST100:
#ifdef GG_CONFIG_HAVE_ZLIB
		return 1;
#else
		return 0;
#endif

	/* Celowo nie ma default, żeby kompilator wyłapał brakujące funkcje */

	}

	return 0;
}