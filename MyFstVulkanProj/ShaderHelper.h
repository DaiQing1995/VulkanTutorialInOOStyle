#ifndef SHADER_HELPER_H
#define SHADER_HELPER_H

#include <fstream>
#include <vector>
#include <stdexcept>

class ShaderHelper
{
public:
	static std::vector<char> readFile(const std::string& filename) {
		// ate:: start reading at the end of file
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		// Since reading is from the EOF, size can be easily acquired
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		// back to beginning of file
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
};

#endif