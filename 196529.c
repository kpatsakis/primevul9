void CLASS colorcheck()
{
#define NSQ 24
// Coordinates of the GretagMacbeth ColorChecker squares
// width, height, 1st_column, 1st_row
  int cut[NSQ][4];			// you must set these
// ColorChecker Chart under 6500-kelvin illumination
  static const double gmb_xyY[NSQ][3] = {
    { 0.400, 0.350, 10.1 },		// Dark Skin
    { 0.377, 0.345, 35.8 },		// Light Skin
    { 0.247, 0.251, 19.3 },		// Blue Sky
    { 0.337, 0.422, 13.3 },		// Foliage
    { 0.265, 0.240, 24.3 },		// Blue Flower
    { 0.261, 0.343, 43.1 },		// Bluish Green
    { 0.506, 0.407, 30.1 },		// Orange
    { 0.211, 0.175, 12.0 },		// Purplish Blue
    { 0.453, 0.306, 19.8 },		// Moderate Red
    { 0.285, 0.202, 6.6 },		// Purple
    { 0.380, 0.489, 44.3 },		// Yellow Green
    { 0.473, 0.438, 43.1 },		// Orange Yellow
    { 0.187, 0.129, 6.1 },		// Blue
    { 0.305, 0.478, 23.4 },		// Green
    { 0.539, 0.313, 12.0 },		// Red
    { 0.448, 0.470, 59.1 },		// Yellow
    { 0.364, 0.233, 19.8 },		// Magenta
    { 0.196, 0.252, 19.8 },		// Cyan
    { 0.310, 0.316, 90.0 },		// White
    { 0.310, 0.316, 59.1 },		// Neutral 8
    { 0.310, 0.316, 36.2 },		// Neutral 6.5
    { 0.310, 0.316, 19.8 },		// Neutral 5
    { 0.310, 0.316, 9.0 },		// Neutral 3.5
    { 0.310, 0.316, 3.1 } };		// Black
  double gmb_cam[NSQ][4], gmb_xyz[NSQ][3];
  double inverse[NSQ][3], cam_xyz[4][3], num;
  int c, i, j, k, sq, row, col, count[4];

  memset (gmb_cam, 0, sizeof gmb_cam);
  for (sq=0; sq < NSQ; sq++) {
    FORCC count[c] = 0;
    for   (row=cut[sq][3]; row < cut[sq][3]+cut[sq][1]; row++)
      for (col=cut[sq][2]; col < cut[sq][2]+cut[sq][0]; col++) {
	c = FC(row,col);
	if (c >= colors) c -= 2;
	gmb_cam[sq][c] += BAYER(row,col);
	count[c]++;
      }
    FORCC gmb_cam[sq][c] = gmb_cam[sq][c]/count[c] - black;
    gmb_xyz[sq][0] = gmb_xyY[sq][2] * gmb_xyY[sq][0] / gmb_xyY[sq][1];
    gmb_xyz[sq][1] = gmb_xyY[sq][2];
    gmb_xyz[sq][2] = gmb_xyY[sq][2] *
		(1 - gmb_xyY[sq][0] - gmb_xyY[sq][1]) / gmb_xyY[sq][1];
  }
  pseudoinverse (gmb_xyz, inverse, NSQ);
  for (raw_color = i=0; i < colors; i++)
    for (j=0; j < 3; j++)
      for (cam_xyz[i][j] = k=0; k < NSQ; k++)
	cam_xyz[i][j] += gmb_cam[k][i] * inverse[k][j];
  cam_xyz_coeff (rgb_cam, cam_xyz);
  if (verbose) {
    printf ("    { \"%s %s\", %d,\n\t{", make, model, black);
    num = 10000 / (cam_xyz[1][0] + cam_xyz[1][1] + cam_xyz[1][2]);
    FORCC for (j=0; j < 3; j++)
      printf ("%c%d", (c | j) ? ',':' ', (int) (cam_xyz[c][j] * num + 0.5));
    puts (" } },");
  }
#undef NSQ
}