#include "SC_Compiler.h"

namespace SC
{

	void SC_Compiler::CompileForVulkan(const std::string& pShader)
	{
		bool pRun = true;
		std::string line = "";
		uint32_t pID = 0;

		while (pRun)
		{
			pID++;

			if (pShader[pID] == ' ' || pShader[pID] == '\t')
				continue;

			line += pShader[pID];

			if (pShader[pID] == ';')
			{
				pAfterComp += TranslateToVulkan(line);
				line = "";
			}
			else if (pID + 1 >= pShader.length())
			{
				pAfterComp += line;
				line = "";
				pRun = false;
			}
		}
	}

	std::string SC_Compiler::TranslateToVulkan(const std::string& pShader)
	{
		if (pShader == "binding=0buffer;")
		{
			return "layout (binding = 0) readonly buffer Buffer { VertexData vertices[]; } in_vertices;";
		}
		else if (pShader == "")
		{

		}

		return "";
	}

}