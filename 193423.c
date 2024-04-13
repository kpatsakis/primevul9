mono_thread_get_stack_bounds (guint8 **staddr, size_t *stsize)
{
#if defined(HAVE_PTHREAD_GET_STACKSIZE_NP) && defined(HAVE_PTHREAD_GET_STACKADDR_NP)
	*staddr = (guint8*)pthread_get_stackaddr_np (pthread_self ());
	*stsize = pthread_get_stacksize_np (pthread_self ());
	*staddr = (guint8*)((gssize)*staddr & ~(mono_pagesize () - 1));
	return;
	/* FIXME: simplify the mess below */
#elif !defined(HOST_WIN32)
	pthread_attr_t attr;
	guint8 *current = (guint8*)&attr;

	pthread_attr_init (&attr);
#  ifdef HAVE_PTHREAD_GETATTR_NP
	pthread_getattr_np (pthread_self(), &attr);
#  else
#    ifdef HAVE_PTHREAD_ATTR_GET_NP
	pthread_attr_get_np (pthread_self(), &attr);
#    elif defined(sun)
	*staddr = NULL;
	pthread_attr_getstacksize (&attr, &stsize);
#    elif defined(__OpenBSD__)
	stack_t ss;
	int rslt;

	rslt = pthread_stackseg_np(pthread_self(), &ss);
	g_assert (rslt == 0);

	*staddr = (guint8*)((size_t)ss.ss_sp - ss.ss_size);
	*stsize = ss.ss_size;
#    else
	*staddr = NULL;
	*stsize = 0;
	return;
#    endif
#  endif

#  if !defined(sun)
#    if !defined(__OpenBSD__)
	pthread_attr_getstack (&attr, (void**)staddr, stsize);
#    endif
	if (*staddr)
		g_assert ((current > *staddr) && (current < *staddr + *stsize));
#  endif

	pthread_attr_destroy (&attr);
#else
	*staddr = NULL;
	*stsize = (size_t)-1;
#endif

	/* When running under emacs, sometimes staddr is not aligned to a page size */
	*staddr = (guint8*)((gssize)*staddr & ~(mono_pagesize () - 1));
}	