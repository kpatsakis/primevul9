init_ft_lib (void)
{
	static gsize ft_init = 0;

	if (g_once_init_enter (&ft_init)) {
		FT_Init_FreeType (&ft_lib);

		g_once_init_leave (&ft_init, (gsize)1);
	}
}