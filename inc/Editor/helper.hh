#pragma once

#include <string>

bool addProgramToRuntime(unsigned char* zipContent, unsigned int zipLength, const std::string& filePath, const std::string& zipPath, const std::string& outputPath);
