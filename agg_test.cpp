#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#include "agg_renderer.h"
#include "agg_rasterizer.h"

enum
{
	width = 400,
	height = 300
};


double random(double min, double max)
{
	int r = (rand() << 15) | rand();
	return ((r & 0xFFFFFFF) / double(0xFFFFFFF + 1)) * (max - min) + min;
}



int main()
{
	// Allocate the framebuffer
	unsigned char* buf = new unsigned char[width * height * 3];

	// Create the rendering buffer 
	agg::rendering_buffer rbuf(buf, width, height, width * 3);

	// Create the renderer and the rasterizer
	agg::renderer<agg::span_rgb24> ren(rbuf);
	agg::rasterizer ras;

	// Setup the rasterizer
	//ras.gamma(1.3);
	ras.filling_rule(agg::fill_even_odd);

	ren.clear(agg::rgba8(0, 0, 0));





	ras.move_to_d(100, 100);
	ras.line_to_d(150, 250);
	ras.line_to_d(200, 200);
	ras.line_to_d(200, 100); 
	ras.line_to_d(-200, -100);

	// Render
	ras.render(ren, agg::rgba8(0xFF, 0xFF, 0xFF, 0xFF));




	// Write a .ppm file
	FILE* fd = fopen("image_test.ppm", "wb");
	fprintf(fd, "P6\n%d %d\n255\n", rbuf.width(), rbuf.height());
	fwrite(buf, 1, rbuf.width() * rbuf.height() * 3, fd);
	fclose(fd);

	delete[] buf;
	return 0;
}

