static void sort_parity_stripes(struct btrfs_bio *bbio, int num_stripes)
{
	struct btrfs_bio_stripe s;
	int i;
	u64 l;
	int again = 1;

	while (again) {
		again = 0;
		for (i = 0; i < num_stripes - 1; i++) {
			if (parity_smaller(bbio->raid_map[i],
					   bbio->raid_map[i+1])) {
				s = bbio->stripes[i];
				l = bbio->raid_map[i];
				bbio->stripes[i] = bbio->stripes[i+1];
				bbio->raid_map[i] = bbio->raid_map[i+1];
				bbio->stripes[i+1] = s;
				bbio->raid_map[i+1] = l;

				again = 1;
			}
		}
	}
}