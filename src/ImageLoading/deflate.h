#include <iostream>

#include "puff.h"

#define LOW_NIBBLE(byte) ((byte)&0xf)
#define HIGH_NIBBLE(byte) ((byte)>>4&0xf)

constexpr char* BOOLSTRING[]{"False", "True"};

uint32_t adler32(uint8_t* buf, uint32_t len, uint32_t adler = 1);

// -------------  TODO: Winapi Window shit and OVERLAY

uint8_t* decompress(uint8_t* data, uint32_t length, uint32_t& resultLength) {
	uint32_t decompressedLength = length;
	resultLength = 1024 * 1024 * 4;
	uint8_t* result = new uint8_t[resultLength];
	int puffRes = puff(result, (unsigned long*)&resultLength, data+2, (unsigned long*)&decompressedLength);
	std::cout << "\nPuff:\n";
	std::cout << "Puff Result: " << puffRes << "\n";
	std::cout << "Puff result length: " << resultLength << "\n";
	std::cout << "\n";

	uint8_t CMF = data[0];
	uint8_t CM = LOW_NIBBLE(CMF); // 0th - 4th bit
	uint8_t CINFO = HIGH_NIBBLE(CMF); // 5th - 7th bit

	uint8_t FLG = data[1];
	// uint8_t FCHECK = LOW_NIBBLE(FLG); // 0th - 4th bit
	bool FCHECK = (data[0] << 8 | data[1]) % 31 == 0;
	bool FDICT = FLG >> 5 & 0x1; // 5th bit
	uint8_t FLEVEL = FLG >> 6 & 0b11; // 6th - 7th bit

	uint32_t ADLER32 = data[length-1 - 3] << 24
					 | data[length-1 - 2] << 16
					 | data[length-1 - 1] << 8
					 | data[length-1 - 0] << 0;
	uint32_t expectedAdler32 = adler32(result, resultLength);
	// std::cout << "Length: " << length << "\n";
	std::cout << "Adler32: " << std::hex << ADLER32 << " Expected: " << expectedAdler32 << std::dec << "\n";

	// if(FDICT) { //TODO handle situation
	// 	std::logic_error("FDICT not handled yet (deflate)");
	// }

	std::cout << "Compression Method: " << (uint16_t)CM << " (8=deflate with window size up to 32K (used by PNG))" << "\n";
	std::cout << "Compression Info: " << (uint16_t)CINFO << " (CINFO=7 indicates a 32K LZZ77 window size)" << "\n";

	std::cout << "FCHECK passed: " << BOOLSTRING[FCHECK] << " (for verification purposes)" << "\n";
	std::cout << "Preset Dictionary: " << BOOLSTRING[FDICT] << " (True => Dictionary is present after FLG Byte)" << "\n";
	const char* COMP_LVL[] {
		" (compressor used fastest algorithm)",
		" (compressor used fast algorithm)",
		" (compressor used default algorithm)",
		" (compressor used maximum compression, slowest algorithm)"};
	std::cout << "Compression Level: " << (uint16_t)FLEVEL << COMP_LVL[FLEVEL] << "\n";

	return result;

	// uint64_t bitIndex = 8*2; // skip CMF and FLG
	// auto getBit = [](uint8_t* data, uint64_t bitIndex)->uint8_t{return data[bitIndex/8]>>(bitIndex%8)&0x1;};

	// bool lastBlock = false;
	// while(!lastBlock) {
	// 	bool BFINAL = getBit(data, bitIndex++);
	// 	uint8_t BTYPE = data[bitIndex/8]>>(bitIndex%8)&0b11; // 00 - no compression, 01 compressed w/ fixed Huffman codes, 10 compressed w/ dynamic Huffman codes, 11 reserved (error)
	// 	bitIndex += 2;
	// 	std::cout << "BFINAL: " << (uint16_t)BFINAL << "\n";
	// 	std::cout << "BTYPE: " << (uint16_t)BTYPE << "\n";


	// 	switch(BTYPE) {
	// 		case 0b00: // 00 - no compression
	// 			{   // 0 1   2  3   4...
	// 				// LEN | NLEN | LEN bytes of uncompressed data
	// 				// uint32_t nextByteIndex = bitIndex/8 + ((bitIndex%8!=0) ? 1 : 0);
	// 			}
	// 			break;
	// 		case 0b01: // 01 compressed w/ fixed Huffman codes
	// 			break;
	// 		case 0b10: // 10 compressed w/ dynamic Huffman codes
	// 			break;
	// 		case 0b11: // 11 reserved (error)
	// 			std::logic_error("BTYPE = 11 used");
	// 			break;
	// 	}

	// 	lastBlock = true;
	// }
}

uint32_t adler32(uint8_t* buf, uint32_t len, uint32_t adler) {
	static constexpr uint32_t BASE = 65521;
	uint32_t s1 = adler & 0xffff;
	uint32_t s2 = (adler >> 16) & 0xffff;

	for (uint32_t i = 0; i < len; i++) {
		s1 = (s1 + buf[i]) % BASE;
		s2 = (s2 + s1)     % BASE;
	}
	return (s2 << 16) + s1;
}

// #define MAXBITS 15              /* maximum bits in a code */
// int construct_asdf(struct huffman *h, const short *length, int n) {
//     int symbol;         /* current symbol when stepping through length[] */
//     int len;            /* current length when stepping through h->count[] */
//     int left;           /* number of possible codes left of current length */
//     short offs[MAXBITS+1];      /* offsets in symbol table for each length */

//     /* count number of codes of each length */
//     for (len = 0; len <= MAXBITS; len++)
//         h->count[len] = 0;
//     for (symbol = 0; symbol < n; symbol++)
//         (h->count[length[symbol]])++;   /* assumes lengths are within bounds */
//     if (h->count[0] == n)               /* no codes! */
//         return 0;                       /* complete, but decode() will fail */

//     /* check for an over-subscribed or incomplete set of lengths */
//     left = 1;                           /* one possible code of zero length */
//     for (len = 1; len <= MAXBITS; len++) {
//         left <<= 1;                     /* one more bit, double codes left */
//         left -= h->count[len];          /* deduct count from possible codes */
//         if (left < 0)
//             return left;                /* over-subscribed--return negative */
//     }                                   /* left > 0 means incomplete */

//     /* generate offsets into symbol table for each length for sorting */
//     offs[1] = 0;
//     for (len = 1; len < MAXBITS; len++)
//         offs[len + 1] = offs[len] + h->count[len];

//     /*
//      * put symbols in table sorted by length, by symbol order within each
//      * length
//      */
//     for (symbol = 0; symbol < n; symbol++)
//         if (length[symbol] != 0)
//             h->symbol[offs[length[symbol]]++] = symbol;

//     /* return zero for complete set, positive for incomplete set */
//     return left;
// }



//Bit stream: Bits int byte sorted LSB -> MSB


// zlib stream:    
//     0     1   
//  | CMF | FLG |

//     0    1    2    3
//  | DICTID(if FLG.FDICT set) |  |..compressed data..|  | ADLER32 |

/*
FCHECK
	The FCHECK value must be such that CMF and FLG, when viewed as
	a 16-bit unsigned integer stored in MSB order (CMF*256 + FLG),
	is a multiple of 31.

FDICT (Preset dictionary)
	If FDICT is set, a DICT dictionary identifier is present
	immediately after the FLG byte. The dictionary is a sequence of
	bytes which are initially fed to the compressor without
	producing any compressed output. DICT is the Adler-32 checksum
	of this sequence of bytes (see the definition of ADLER32
	below).  The decompressor can use this identifier to determine
	which dictionary has been used by the compressor.

compressed data
	For compression method 8, the compressed data is stored in the
	deflate compressed data format as described in the document
	"DEFLATE Compressed Data Format Specification" by L. Peter
	Deutsch. (See reference [3] in Chapter 3, below)

	Other compressed data formats are not specified in this version
	of the zlib specification.
*/
