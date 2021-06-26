#pragma once

#include "winapi_include.h"
#include <cstdint>

class Texture {
public:
    uint32_t *buffer;
    uint32_t width, height;
public:
    Texture(uint32_t width, uint32_t height);
    ~Texture();
    void clear(uint32_t color);
    void setPixel(uint32_t x, uint32_t y, uint32_t color);
    void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
    void fillRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
	void blit(Texture* tex, int32_t x1, int32_t y1);
	void resize(uint32_t width, uint32_t height);
};