#include "Texture.h"

#include <cmath>
#include <algorithm>

Texture::Texture(uint32_t width, uint32_t height): width(width), height(height) {
    this->buffer = new uint32_t[width*height];
}

Texture::~Texture() {
    delete[] buffer;
}

void Texture::clear(uint32_t color) {
    for (uint32_t i = 0; i < this->width * this->height; i++)
        this->buffer[i] = color;
}

void Texture::setPixel(uint32_t x, uint32_t y, uint32_t color) {
    // if((x|y)<0 || x>=this->width || y>=this->height) return;
    if(x>=this->width || y>=this->height) return;
    this->buffer[y * this->width + x] = color;
}

void Texture::line(int32_t x1_in, int32_t y1_in, int32_t x2_in, int32_t y2_in, uint32_t color) {
    auto map = [](float v, float vMin, float vMax, float outMin, float outMax)->float{ float t=(v-vMin)/(vMax-vMin); return outMin+(outMax-outMin)*t; };
	y1_in = y1_in;// Invert y axis
	y2_in = y2_in;
    int dx = x2_in - x1_in;
    int dy = y2_in - y1_in;
    bool hor = abs(dx) > abs(dy);
    if(hor) {
        int x1 = std::min(x1_in, x2_in);
        int x2 = std::max(x1_in, x2_in);
        int y1 = (x1_in < x2_in) ? y1_in : y2_in;
        int y2 = (x1_in < x2_in) ? y2_in : y1_in;
        for(int x=std::max(x1, 0); x<x2; x++) {
            setPixel(x, (int)map(x, x1, x2, y1, y2), color);
			if(x >= (int)width-1)
				break;
        }
    } else {
        int y1 = std::min(y1_in, y2_in);
        int y2 = std::max(y1_in, y2_in);
        int x1 = (y1_in < y2_in) ? x1_in : x2_in;
        int x2 = (y1_in < y2_in) ? x2_in : x1_in;
        for(int y=std::max(y1, 0); y<y2; y++) {
            setPixel((int)map(y, y1, y2, x1, x2), y, color);
			if(y >= (int)height-1)
				break;
        }
    }
}

void Texture::fillRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
	for(uint32_t y=std::max(y1, 0); y<std::min(y2, (int32_t)this->height); y++)
		for(uint32_t x=std::max(x1, 0); x<std::min(x2, (int32_t)this->width); x++)
			this->buffer[y * this->width + x] = color;
}

void Texture::blit(Texture* tex, int32_t x1, int32_t y1) {
	for(uint32_t y=0; y<tex->height; y++) {
		for(uint32_t x=0; x<tex->width; x++) {
			if(x1 + x < 0 || y1 + y < 0)
				continue;
			uint32_t xTarget = x1 + x;
			uint32_t yTarget = y1 + y;
			if(xTarget < this->width && yTarget < this->height)
				buffer[yTarget * this->width + xTarget] = tex->buffer[(tex->height-1-y)*tex->width+x];
		}
	}
}

void Texture::resize(uint32_t _width, uint32_t _height) {
	this->width = _width;
	this->height = _height;
	delete[] this->buffer;
	this->buffer = new uint32_t[_width * _height];
}