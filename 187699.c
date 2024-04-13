static void delay(int ms)
{
	unsigned long timeout = jiffies + ((ms * HZ) / 1000);
	while (time_before(jiffies, timeout))
		cpu_relax();
}