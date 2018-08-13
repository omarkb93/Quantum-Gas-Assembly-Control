#pragma once
#include <chrono>
#include <unordered_map>
#include "commonTypes.h"

class CodeTimer
{
	public:
		void tick(std::string timeName);
		std::string getTimingMessage(bool ms = true);
	private:
		std::vector<std::pair<std::string, chronoTime>> times;
};

