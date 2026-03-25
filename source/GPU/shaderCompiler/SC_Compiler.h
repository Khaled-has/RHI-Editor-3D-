#ifndef SC_COMPILER_H
#define SC_COMPILER_H

#include <iostream>
#include <vector>
#include <string>

namespace SC
{

	class SC_Compiler
	{
	public:
		SC_Compiler() {}
		~SC_Compiler() {}

		void CompileForVulkan(const std::string& pShader);

		inline std::string GetShader() { return pAfterComp; }

	private:
		std::string pAfterComp = "";

		std::string TranslateToVulkan(const std::string& pShader);
	};

}

#endif