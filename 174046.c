static inline size_t SALEN(struct sockaddr *sa) {
	switch (sa->sa_family) {
	case AF_INET:  return (sizeof(struct sockaddr_in));
	case AF_INET6: return (sizeof(struct sockaddr_in6));
	default:       return 0;
	}
}