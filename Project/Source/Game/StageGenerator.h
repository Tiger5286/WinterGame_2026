#pragma once
#include <string>
#include <memory>
#include "SpawnContext.h"

namespace StageGenerator
{
	void Generate(const SpawnContext::Info& info, const std::wstring& folderPath);
}