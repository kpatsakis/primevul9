static struct hlist_head *rds_conn_bucket(const struct in6_addr *laddr,
					  const struct in6_addr *faddr)
{
	static u32 rds6_hash_secret __read_mostly;
	static u32 rds_hash_secret __read_mostly;

	u32 lhash, fhash, hash;

	net_get_random_once(&rds_hash_secret, sizeof(rds_hash_secret));
	net_get_random_once(&rds6_hash_secret, sizeof(rds6_hash_secret));

	lhash = (__force u32)laddr->s6_addr32[3];
#if IS_ENABLED(CONFIG_IPV6)
	fhash = __ipv6_addr_jhash(faddr, rds6_hash_secret);
#else
	fhash = (__force u32)faddr->s6_addr32[3];
#endif
	hash = __inet_ehashfn(lhash, 0, fhash, 0, rds_hash_secret);

	return &rds_conn_hash[hash & RDS_CONNECTION_HASH_MASK];
}