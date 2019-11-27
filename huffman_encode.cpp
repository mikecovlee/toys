#define __DEBUG__
#include "huffman.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		return -1;
	huffman_compress::compress(argv[1], argv[2]);
	return 0;
}