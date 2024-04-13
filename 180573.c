static inline int guestwidth_to_adjustwidth(int gaw)
{
	int agaw;
	int r = (gaw - 12) % 9;

	if (r == 0)
		agaw = gaw;
	else
		agaw = gaw + 9 - r;
	if (agaw > 64)
		agaw = 64;
	return agaw;
}