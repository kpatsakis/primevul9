static void add_interrupt_bench(cycles_t start)
{
        long delta = random_get_entropy() - start;

        /* Use a weighted moving average */
        delta = delta - ((avg_cycles + FIXED_1_2) >> AVG_SHIFT);
        avg_cycles += delta;
        /* And average deviation */
        delta = abs(delta) - ((avg_deviation + FIXED_1_2) >> AVG_SHIFT);
        avg_deviation += delta;
}