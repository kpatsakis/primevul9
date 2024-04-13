static inline float point_inside_circle(float px, float py, float x, float y, float r)
{
	float dx = px - x;
	float dy = py - y;
	return dx * dx + dy * dy <= r * r;
}