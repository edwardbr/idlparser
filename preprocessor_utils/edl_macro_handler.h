#pragma once

#include "macrohandler.h"

struct edl_macro_parser : public macro_parser
{
	edl_macro_parser();
    [[nodiscard]] bool ParseInclude(const char*& pData, int ignoreText, std::ostream& dest, const paths& includeDirectories, std::vector<std::string>& loaded_includes) override;
};