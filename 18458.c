void add_interrupt_randomness(int irq, int irq_flags)
{
	struct entropy_store	*r;
	struct fast_pool	*fast_pool = this_cpu_ptr(&irq_randomness);
	struct pt_regs		*regs = get_irq_regs();
	unsigned long		now = jiffies;
	cycles_t		cycles = random_get_entropy();
	__u32			c_high, j_high;
	__u64			ip;
	unsigned long		seed;
	int			credit = 0;

	if (cycles == 0)
		cycles = get_reg(fast_pool, regs);
	c_high = (sizeof(cycles) > 4) ? cycles >> 32 : 0;
	j_high = (sizeof(now) > 4) ? now >> 32 : 0;
	fast_pool->pool[0] ^= cycles ^ j_high ^ irq;
	fast_pool->pool[1] ^= now ^ c_high;
	ip = regs ? instruction_pointer(regs) : _RET_IP_;
	fast_pool->pool[2] ^= ip;
	fast_pool->pool[3] ^= (sizeof(ip) > 4) ? ip >> 32 :
		get_reg(fast_pool, regs);

	fast_mix(fast_pool);
	add_interrupt_bench(cycles);
	this_cpu_add(net_rand_state.s1, fast_pool->pool[cycles & 3]);

	if (unlikely(crng_init == 0)) {
		if ((fast_pool->count >= 64) &&
		    crng_fast_load((char *) fast_pool->pool,
				   sizeof(fast_pool->pool))) {
			fast_pool->count = 0;
			fast_pool->last = now;
		}
		return;
	}

	if ((fast_pool->count < 64) &&
	    !time_after(now, fast_pool->last + HZ))
		return;

	r = &input_pool;
	if (!spin_trylock(&r->lock))
		return;

	fast_pool->last = now;
	__mix_pool_bytes(r, &fast_pool->pool, sizeof(fast_pool->pool));

	/*
	 * If we have architectural seed generator, produce a seed and
	 * add it to the pool.  For the sake of paranoia don't let the
	 * architectural seed generator dominate the input from the
	 * interrupt noise.
	 */
	if (arch_get_random_seed_long(&seed)) {
		__mix_pool_bytes(r, &seed, sizeof(seed));
		credit = 1;
	}
	spin_unlock(&r->lock);

	fast_pool->count = 0;

	/* award one bit for the contents of the fast pool */
	credit_entropy_bits(r, credit + 1);
}