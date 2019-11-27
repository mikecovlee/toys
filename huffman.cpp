#define __DEBUG__
#include "huffman.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cerr << "Wrong argument." << std::endl;
		return -1;
	}
	std::string cmd(argv[1]);
	if (cmd == "encode" && argc == 4) {
		try {
			huffman_compress::compress(argv[2], argv[3]);
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return -1;
		}
	}
	else if (cmd == "decode" && argc == 4) {
		try {
			huffman_compress::decompress(argv[2], argv[3]);
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << std::endl;
			return -1;
		}
	}
	else if (cmd == "info" && argc == 3) {
		try {
			auto info = huffman_compress::read_info(argv[2]);
			std::cout << "Index Count: " << (int)info.index_count + 1 << std::endl;
			std::cout << "Checksum: " << info.checksum << std::endl;
			std::cout << "DictData Size: " << info.dict_dat_size << std::endl;
			std::cout << "DictDat Align: " << (int)info.dict_dat_align << std::endl;
			std::cout << "FileData Size: " << info.file_dat_size << std::endl;
			std::cout << "FileDat Align: " << (int)info.file_dat_align << std::endl;
		}
		catch(...) {
			std::cerr << "Not a huffman archive." << std::endl;
			return -1;
		}
	}
	else {
		std::cerr << "Wrong argument." << std::endl;
		return -1;
	}
    std::cout << "Operation complete." << std::endl;
	return 0;
}