    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0, const int z0,
                            const int x1, const int y1, const int z1,
                            const tc *const color, const float opacity,
                            const unsigned int pattern) {
      return draw_line(x0,y0,z0,x1,y0,z0,color,opacity,pattern,true).
	draw_line(x1,y0,z0,x1,y1,z0,color,opacity,pattern,false).
	draw_line(x1,y1,z0,x0,y1,z0,color,opacity,pattern,false).
	draw_line(x0,y1,z0,x0,y0,z0,color,opacity,pattern,false).
	draw_line(x0,y0,z1,x1,y0,z1,color,opacity,pattern,true).
	draw_line(x1,y0,z1,x1,y1,z1,color,opacity,pattern,false).
	draw_line(x1,y1,z1,x0,y1,z1,color,opacity,pattern,false).
	draw_line(x0,y1,z1,x0,y0,z1,color,opacity,pattern,false).
	draw_line(x0,y0,z0,x0,y0,z1,color,opacity,pattern,true).
	draw_line(x1,y0,z0,x1,y0,z1,color,opacity,pattern,true).
	draw_line(x1,y1,z0,x1,y1,z1,color,opacity,pattern,true).
	draw_line(x0,y1,z0,x0,y1,z1,color,opacity,pattern,true);