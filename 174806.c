des (guint32 ks[16][2], unsigned char block[8])
{
	guint32 left,right,work;
	
	/* Read input block and place in left/right in big-endian order */
	left = ((guint32)block[0] << 24)
	 | ((guint32)block[1] << 16)
	 | ((guint32)block[2] << 8)
	 | (guint32)block[3];
	right = ((guint32)block[4] << 24)
	 | ((guint32)block[5] << 16)
	 | ((guint32)block[6] << 8)
	 | (guint32)block[7];

	/* Hoey's clever initial permutation algorithm, from Outerbridge
	 * (see Schneier p 478)	
	 *
	 * The convention here is the same as Outerbridge: rotate each
	 * register left by 1 bit, i.e., so that "left" contains permuted
	 * input bits 2, 3, 4, ... 1 and "right" contains 33, 34, 35, ... 32	
	 * (using origin-1 numbering as in the FIPS). This allows us to avoid
	 * one of the two rotates that would otherwise be required in each of
	 * the 16 rounds.
	 */
	work = ((left >> 4) ^ right) & 0x0f0f0f0f;
	right ^= work;
	left ^= work << 4;
	work = ((left >> 16) ^ right) & 0xffff;
	right ^= work;
	left ^= work << 16;
	work = ((right >> 2) ^ left) & 0x33333333;
	left ^= work;
	right ^= (work << 2);
	work = ((right >> 8) ^ left) & 0xff00ff;
	left ^= work;
	right ^= (work << 8);
	right = (right << 1) | (right >> 31);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left << 1) | (left >> 31);

	/* Now do the 16 rounds */
	F(left,right,ks[0]);
	F(right,left,ks[1]);
	F(left,right,ks[2]);
	F(right,left,ks[3]);
	F(left,right,ks[4]);
	F(right,left,ks[5]);
	F(left,right,ks[6]);
	F(right,left,ks[7]);
	F(left,right,ks[8]);
	F(right,left,ks[9]);
	F(left,right,ks[10]);
	F(right,left,ks[11]);
	F(left,right,ks[12]);
	F(right,left,ks[13]);
	F(left,right,ks[14]);
	F(right,left,ks[15]);

	/* Inverse permutation, also from Hoey via Outerbridge and Schneier */
	right = (right << 31) | (right >> 1);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left >> 1) | (left  << 31);
	work = ((left >> 8) ^ right) & 0xff00ff;
	right ^= work;
	left ^= work << 8;
	work = ((left >> 2) ^ right) & 0x33333333;
	right ^= work;
	left ^= work << 2;
	work = ((right >> 16) ^ left) & 0xffff;
	left ^= work;
	right ^= work << 16;
	work = ((right >> 4) ^ left) & 0x0f0f0f0f;
	left ^= work;
	right ^= work << 4;

	/* Put the block back into the user's buffer with final swap */
	block[0] = right >> 24;
	block[1] = right >> 16;
	block[2] = right >> 8;
	block[3] = right;
	block[4] = left >> 24;
	block[5] = left >> 16;
	block[6] = left >> 8;
	block[7] = left;
}