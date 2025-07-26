#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <mruby.h>
#include <mruby/compile.h>
#include <Editor/WindowZone.hh>
#include <Editor/TitleBarMode.hh>

class ProgramPod {
public:
	ProgramPod(const std::string& programPath);
	~ProgramPod();

protected:

	std::string _programFile;
	std::vector<std::string> _programParameters;
};
