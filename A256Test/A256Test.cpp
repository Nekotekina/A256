// A256Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <codecvt>

#include "../A256Core/A256Interpreter.h"

A256Machine vm;

int _tmain(int argc, _TCHAR* argv[])
{
	std::string text =
		"#cnt 0x00ffffff; abc\n"
		"set         $01.0x93, #cnt; initialize counter\n"
		"@SimpleLoop:\n"
		"subd        $01, $01, 1; decrement\n"
		"jnzd        $01.ud0, @SimpleLoop; test and jump\n"
		"stop        $01, 0xB;\n";
	try
	{
		if (argc > 1)
		{
			std::wstring_convert<std::codecvt_utf8<_TCHAR>, _TCHAR> convert;
			std::string name = convert.to_bytes(argv[1]);
			std::ifstream t(argv[1]);
			if (t.is_open())
			{
				text = std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
				printf("'%s' loaded.\n", name.c_str());
			}
			else
			{
				throw fmt::format("file '%s' not found.", name.c_str());
			}
		}
		else
		{
			printf(text.c_str());
		}
		printf("Compiling...\n");
		std::vector<A256Cmd> program = vm.compile(text);
		printf("%lld instructions generated.\n", program.size());
		printf("Executing...\n");
		vm.reg[0]._uq[0] = (u64)program.data();
		while (vm.execute());
		printf("Program finished.\n");
	}
	catch (size_t& x)
	{
		size_t line = 0;
		size_t column = 0;
		for (size_t i = 0; i < x; i++)
		{
			if (text[i] == '\n')
			{
				line++;
				column = 0;
			}
			else if (text[i] == '\r')
			{
			}
			else
			{
				column++;
			}
		}
		printf("Compilation failed (offset=0x%llx, line %lld, column %lld)\n", x, line + 1, column + 1);
	}
	catch (std::string& x)
	{
		printf("Error: %s\n", x.c_str());
	}
	return 0;
}

