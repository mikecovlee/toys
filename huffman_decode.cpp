#include "huffman.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
		return -1;
	huffman_compress::decompress(argv[1], argv[2]);
	return 0;
}