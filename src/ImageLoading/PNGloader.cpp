#include "PNGloader.h"

#include "deflate.h"

#include <iostream>
#include <fstream>

namespace {
	uint8_t* readFile(const char* path, uint32_t& length) {
		std::ifstream file;
		file.open(path, std::ios::binary);
		if(!file.is_open())
			return nullptr;
		file.seekg (0, std::ios::end);
		length = file.tellg();
		file.seekg (0, std::ios::beg);
		uint8_t* data = new uint8_t[length];
		file.read((char*)data, length);
		file.close();
		return data;
	}

	uint8_t read8(const uint8_t* data, uint32_t& offset) {
		return data[offset++];
	}

	// uint16_t read16(const uint8_t* data, uint32_t& offset) {
	// 	uint16_t out = data[offset++] << 8;
	// 			out |= data[offset++] << 0;
	// 	return out;
	// }

	uint32_t read32(const uint8_t* data, uint32_t& offset) {
		uint32_t out = data[offset++] << 24;
				out |= data[offset++] << 16;
				out |= data[offset++] << 8;
				out |= data[offset++] << 0;
		return out;
	}
}

constexpr uint32_t MakeCC(const char* str) {
    // if(str.length() != 4)
    //     throw std::logic_error("str must be length 4");
    // return str[0] | str[1] << 8 | str[2] << 16 | str[3] << 24;
    return str[0]<<24 | str[1] << 16 | str[2] << 8 | str[3] << 0;
    // return *(uint32_t*)str;
}

static constexpr uint32_t IHDR_NUM = MakeCC("IHDR"); // IHDR
static constexpr uint32_t PLTE_NUM = MakeCC("PLTE"); // PLTE
static constexpr uint32_t IDAT_NUM = MakeCC("IDAT"); // IDAT
static constexpr uint32_t IEND_NUM = MakeCC("IEND"); // IEND

//ancillary
// static constexpr uint32_t BKND_NUM = MakeCC("bKGD"); // bKGD
// static constexpr uint32_t CHRM_NUM = MakeCC("cHRM"); // cHRM
// static constexpr uint32_t GAMA_NUM = MakeCC("gAMA"); // gAMA
// static constexpr uint32_t HIST_NUM = MakeCC("hIST"); // hIST
// static constexpr uint32_t PHYS_NUM = MakeCC("pHYs"); // pHYs
// static constexpr uint32_t SBIT_NUM = MakeCC("sBIT"); // sBIT
// static constexpr uint32_t TEXT_NUM = MakeCC("tEXt"); // tEXt
// static constexpr uint32_t TIME_NUM = MakeCC("tIME"); // tIME
// static constexpr uint32_t TRNS_NUM = MakeCC("tRNS"); // tRNS
// static constexpr uint32_t ZTXT_NUM = MakeCC("zTXt"); // zTXt

Texture* loadPNG(const char* path) {
	uint32_t length;
	uint8_t* data = readFile(path, length);

	struct {
		uint32_t width;
		uint32_t height;
		uint8_t bit_depth;
		uint8_t color_type;
		uint8_t compression_method;
		uint8_t filter_method;
		uint8_t interlace_method;
	} meta;

	struct {
		uint32_t length;
		uint32_t type;
		uint8_t* data;
		uint32_t crc;
	} chunk;

	struct DataStream {
		uint32_t length;
		uint8_t* data;
		DataStream* next;
		DataStream(uint32_t length, uint8_t* data, DataStream* next): length(length), data(data), next(next) { }
	};
	DataStream* dataStream;

	DataStream** dataStreamEnd = &dataStream; // only used fer appending stream chunks
	uint32_t dataStreamLength = 0;

	Texture* tex;

	uint32_t filePos = 8; // First 8 Bytes: 137 80 78 71 13 10 26 10 (decimal)
	bool done = false;
	while(!done) {
		chunk.length = read32(data, filePos);
		chunk.type = read32(data, filePos);

		chunk.data = data + filePos;
		filePos += chunk.length;

		chunk.crc = read32(data, filePos);

		uint32_t chunkOffset = 0;
		switch(chunk.type) {
			case IHDR_NUM:
				meta.width = read32(chunk.data, chunkOffset);
				meta.height = read32(chunk.data, chunkOffset);

				meta.bit_depth = read8(chunk.data, chunkOffset);
				meta.color_type = read8(chunk.data, chunkOffset);
				meta.compression_method = read8(chunk.data, chunkOffset);
				meta.filter_method = read8(chunk.data, chunkOffset);
				meta.interlace_method = read8(chunk.data, chunkOffset);

				std::cout << "IHDR LENGTH: " << chunk.length << "\n";
				std::cout << "Width: " << meta.width << "\n";
				std::cout << "Height: " << meta.height << "\n";
				std::cout << "Bit Depth: " << (uint16_t)meta.bit_depth << "\n";
				std::cout << "Color Type: " << (uint16_t)meta.color_type << "\n";
				std::cout << "Compression Method: " << (uint16_t)meta.compression_method << "\n";
				std::cout << "Filter Method: " << (uint16_t)meta.filter_method << "\n";
				std::cout << "Interlace Method: " << (uint16_t)meta.interlace_method << "\n";
				std::cout << "\n";
				break;

			case PLTE_NUM:
				std::cout << "PLTE LENGTH: " << chunk.length << "\n";
				std::cout << "\n";
			// 	{
			// 		int numEntries = chunk.length / 3;
			// 		if(chunk.length%3 != 0)
			// 			std::logic_error("Pallette chunk length not divisable by 3.");
			// 		for(int i=0; i<numEntries; i++) {
			// 			uint8_t red = chunk.data[i*3];
			// 			uint8_t green = chunk.data[i*3+1];
			// 			uint8_t blue = chunk.data[i*3+2];
			// 		}
			// 	}
				break;

			case IDAT_NUM:
				*dataStreamEnd = new DataStream(chunk.length, chunk.data, nullptr);
				dataStreamEnd = &((*dataStreamEnd)->next);
				dataStreamLength += chunk.length;

				// std::cout << "IDAT LENGTH: " << chunk.length << "\n";
				// std::cout << "\n";
				break;

			case IEND_NUM:
				// std::cout << "IEND LENGTH: " << chunk.length << "\n";
				// std::cout << "\n";
				done = true;
				break;

			default:
				std::cout << "Unknown Chunk LENGTH: " << chunk.length << "\n";
				char type[5];
				type[4] = 0;
				*(uint32_t*)type = chunk.type;
				std::cout << "Type: " << type << "\n";
				std::cout << "\n";
				done = true;
				break;
		}

		// std::cout << "Length: " << chunk.length << "\n";
		// std::cout << "Type: " << chunk.type << "\n";
		// done = true;
	}

	tex = new Texture(meta.width, meta.height);

	uint32_t decompressedLength;
	uint8_t* decompressed;
	{
		uint8_t* stream = new uint8_t[dataStreamLength];
		uint32_t streamPos = 0;
		for(DataStream* currentChunk=dataStream, *prevChunk; currentChunk!=nullptr; prevChunk=currentChunk, currentChunk=currentChunk->next, delete prevChunk) //{
			for(uint32_t i=0; i<currentChunk->length; i++)
				stream[streamPos++] = currentChunk->data[i];
		decompressed = decompress(stream, streamPos, decompressedLength);
		delete[] stream;
	}



	auto paethPredictor = [](const uint8_t a, const uint8_t b, const uint8_t c) -> uint8_t {
				int16_t p = a + b - c;
				int16_t pa = abs(p - a);
				int16_t pb = abs(p - b);
				int16_t pc = abs(p - c);
				if (pa <= pb && pa <= pc)
					return a;
				if (pb <= pc)
					return b;
				return c;
				};


	// C B
	// A X
	uint16_t channels = -1;
	switch(meta.color_type) {
		case 0: // Grayscale
			channels = 1;
			break;
		case 4: // Grayscale Alpha
			channels = 2;
			break;
		case 2: // R G B
			channels = 3;
			break;
		case 6: // R G B A
			channels = 4;
			break;
		case 3: // Indexed Color
			channels = 1;
			break;
	}

	uint16_t bitsPerPixel = meta.bit_depth * channels;

	auto ind = [meta, channels](int x, int y){ return y * (meta.width*channels + 1) + x * channels + 1; };
	
	for(uint32_t y=0; y<meta.height; y++) {
		uint8_t filter = decompressed[ind(0, y) - 1];
		// std::cout << (uint16_t)filter << "\n";
		for(uint32_t x=0; x<meta.width; x++) {
			for(uint32_t channel=0; channel<channels; channel++) {
				uint32_t indX = ind(x, y);
				uint32_t indA = ind(x-1, y); // left
				uint32_t indB = ind(x,   y-1); // above
				uint32_t indC = ind(x-1, y-1); // above left
				uint8_t& X = decompressed[indX+channel];
				const uint8_t A = (x > 0) ? decompressed[indA+channel] : 0;
				const uint8_t B = (y > 0) ? decompressed[indB+channel] : 0;
				const uint8_t C = (x > 0 && y > 0) ? decompressed[indC+channel] : 0;
				switch(filter) {
					case 1:
						X += A; // Sub
						break;

					case 2:
						X += B; // UP
						break;

					case 3:
						X += (A + B) / 2; // Average
						break;

					case 4:
						X += paethPredictor(A, B, C); // Paeth
						break;
				}
			}
		}
	}

	for(uint32_t y=0; y<meta.height; y++) {
		for(uint32_t x=0; x<meta.width; x++) {
			uint32_t& col = tex->buffer[y * meta.width + x];
			col = 0x00000000;
			switch(meta.color_type) {
				case 0: // Greyscale
				case 2: // Truecolor RGB
					col |= 0xFF000000;// fully opaque
					break;
			}
			switch(meta.bit_depth) {
				case 8:
				uint32_t index = ind(x, meta.height-1-y);
				tex->buffer[y * meta.width + x] |=
					decompressed[index] << 24
					| decompressed[index] << 16
					| decompressed[index + 1] << 8
					| decompressed[index + 2] << 0;
					break;
			}
		}
	}

	std::cout << "File reading done\n";

	return tex;
}