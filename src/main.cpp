#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cmath>

#include "Taskbar.h"
#include "Keyboard.h"
#include "Texture.h"

#include <chrono>

// #include "ImageLoading/BMPloader.h"
// #include "ImageLoading/PNGloader.h"

int main() {
	// Texture* imgTexture = loadBMP("../data/bmp_1.bmp"); // 1 bitPerPixel   monochrome
	// Texture* imgTexture = loadBMP("../data/bmp_4.bmp"); // 4 bitPerPixel   16 colors
	// Texture* imgTexture = loadBMP("../data/bmp_8.bmp"); // 8 bitPerPixel   256 colors
	// Texture* imgTexture = loadBMP("../data/bmp_16.bmp"); // 16 bitPerPixel 255 ^ 2 Colors
	// Texture* imgTexture = loadBMP("../data/bmp_24.bmp"); // 24 bitPerPixel 255 ^ 3 Colors
	// Texture* imgTexture = loadPNG("../data/img1.png");
	// Texture* imgTexture = loadPNG("../data/img2.png");
	// Texture* imgTexture = loadPNG("../data/trolltime.png");

    Taskbar taskbar(800, 50);

    int32_t pmouseX = 0, pmouseY = 0;
    int64_t lastTime = 0;
    while (taskbar.pollMsg()) {
        Keyboard::poll();


		const uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		int64_t dt_millis = time - lastTime;
		float dt = dt_millis / 1000.;
		float fps = 1000. / dt_millis;
        // std::cout << "" << fps << "\n";
        // lastTime = time;


        // if (Keyboard::pressed('W'))
        //     std::cout << "W\n";
        // if (Keyboard::pressed('A'))
        //     std::cout << "A\n";
        // if (Keyboard::pressed('S'))
        //     std::cout << "S\n";
        // if (Keyboard::pressed(0x01)) // left click
        //     std::cout << "left\n";
        // if (Keyboard::pressed(0x02)) // right click
        //     std::cout << "right\n";

		uint8_t brightness = taskbar.mouseX * 255 / taskbar.width;
		uint8_t transparency = std::max<uint8_t>(1, taskbar.mouseY * 255 / taskbar.height);
        taskbar.graphics.clear(transparency<< 24 | (brightness<<16 | brightness<<8 | brightness));// A R G B
        if (Keyboard::pressed('D'))
            std::cout << "Transparency: " << (uint16_t)transparency << " Brightness: " << (uint16_t)brightness << "\n";
		
		// taskbar.graphics.blit(imgTexture, win.mouseX-imgTexture->width/2, win.mouseY-imgTexture->height/2);
		// taskbar.graphics.blit(imgTexture, 0, 20);
		// taskbar.graphics.blit(imgTexture, mousePos.x, mousePos.y);
        taskbar.graphics.line(20, 20, taskbar.mouseX, taskbar.mouseY, 0xff0088ff);
        taskbar.updateScreen();

		pmouseX = taskbar.mouseX;
		pmouseY = taskbar.mouseY;
		// Sleep(6);
    }
    return 0;
}