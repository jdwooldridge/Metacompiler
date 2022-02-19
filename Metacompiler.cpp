//#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "tinyxml/tinyxml.h"
#include "tinyxml/tinystr.h"

int main(int argc, char *argv[])
{
	char* inFilePath = "";
	char* outFilePath = "";
	//Throw usage error message if the number of argument characters is not 5.
	if (argc != 5)
	{
		printf("Usage: '%s -i <PATHTOINPUTFILE> -o <PATHTOOUTPUTFILE>\n", argv[0]);
		return 0;
	}
	else
	{	//Parse the arguments, retrieving an input file path and output file path after a '-i' and '-o' respectively.
		for (int i = 0; i < argc; ++i)
		{
			if (strcmp(argv[i], "-i") == 0)
				inFilePath = argv[i + 1];
			if (strcmp(argv[i], "-o") == 0)
				outFilePath = argv[i + 1];
		}
	}
	//Throw error if neither the input file path not output file paths were defined.
	if (inFilePath == "" || outFilePath == "")
	{
		printf("Error: Could not get the input or output file paths!\n");
		return 0;
	}
	TiXmlDocument doc(inFilePath);
	//Throw error if language definition input file couldn't be loaded.
	if (!doc.LoadFile())
	{
		printf("Could not load input file. Error:'%s'.\n", doc.ErrorDesc());
		return 0;
	}
	//Create XML handler.
	TiXmlHandle hDoc(&doc);
	//Attempt to get the Language node. Throw error if it wasn't found.
	TiXmlElement* root = doc.FirstChildElement("Language");
	if (!root)
	{
		printf("Could not find Language node!\n");
		return 0;
	}
	//Attempt to get the Alphabet child element of Language. Throw error if it couldn't be found.
	TiXmlElement* elem = root->FirstChildElement("Alphabet");
	if (!elem)
	{
		printf("Could not find Alphabet node!\n");
		return 0;
	}
	//Retrieve the alphabetical characters.
	std::vector<char> alpha;
	TiXmlElement* elemChild = elem->FirstChildElement("Letter");
	if (!elemChild)
	{
		printf("Could not find Letter node!\n");
		return 0;
	}
	while (elemChild)
	{
		alpha.push_back(*elemChild->Attribute("char"));
		elemChild = elemChild->NextSiblingElement("Letter");
	}
	//Declare matrix dimensions.
	int numRows;
	int numCols;
	elem = elem->NextSiblingElement("MatrixDims");
	//Throw error if MatrixDims element could not be retrieved.
	if (!elem)
	{
		printf("Could not find MatrixDims node!\n");
		return 0;
	}
	//Otherwise retrieve the matrix dimension information.
	else
	{
		elem->QueryIntAttribute("rows", &numRows);
		elem->QueryIntAttribute("cols", &numCols);
	}
	//Retrieve the transition matrix information.
	elem = elem->NextSiblingElement("Matrix");
	if (!elem)
	{
		printf("Could not find Matrix element!");
		return 0;
	}
	elemChild = elem->FirstChildElement("State");
	if (!elemChild)
	{
		printf("Error: Could not find State element!\n");
		return 0;
	}
	std::vector< std::vector<int> > matrix;
	//Alter the size of the matrix per the dimensions specified.
	matrix.resize(numRows);
	for (int i = 0; i < numRows; ++i)
		matrix[i].resize(numCols);

	//Put the transition matrix elements in the matrix.
	while (elemChild)
	{
		for (int i = 0; i < numRows; ++i)
		{
			for (int j = 0; j < numCols; ++j)
			{
				std::string stateStr = "alpha" + std::to_string(j);
				elemChild->QueryIntAttribute(stateStr.c_str(), &matrix[i][j]);
			}

			elemChild = elemChild->NextSiblingElement("State");
		}
	}
	
	elem = elem->NextSiblingElement("Messages");
	//Throw error if the Messages element could not be retrieved.
	if (!elem)
	{
		printf("Error: Could not find Messages element!\n");
		return 0;
	}
	elemChild = elem->FirstChildElement("Message");
	//Throw error if we could not get the Message element.
	if (!elemChild)
	{
		printf("Error: Could not find Message element!\n");
		return 0;
	}
	//Make map that maps transition states to output messages.
	std::map<int, std::string> messages;
	while (elemChild)
	{
		messages.insert(std::pair<int, std::string>(atoi(elemChild->Attribute("state")), elemChild->Attribute("text")));
		elemChild = elemChild->NextSiblingElement("Message");
	}

	//Begin compiler output.
	printf("Creating compiler...\n");
	std::ofstream fout(outFilePath);

	fout << "#include <iostream>\n#include <string>\n#include <algorithm>" << std::endl;
	/*ofs << "#include <string>" << endl;
	ofs << "#include <algorithm>" << endl << endl;*/
	fout << "int main()\n{" << std::endl;
	fout << "\tint state = 0;" << std::endl;
	fout << "\tchar alpha[" << alpha.size() << "] = {";
	//Output the alphabet array elements.
	auto alphaIter = alpha.begin();
	for (auto& character : alpha)
		if (character != alpha.back())
			fout << "'" << character << "', ";
		else
			fout << "'" << character << "' };" << std::endl;
	//Begin transition matrix output.
	fout << "\tint matrix[" << numRows << "][" << numCols << "] =\n\t{" << std::endl;
	for (int i = 0; i < numRows; ++i)
		for (int j = 0; j < numCols; ++j)
		{
			if (j == 0)
				fout << "\t\t{" << matrix[i][j] << ", ";
			else if (j == numCols - 1 && i != numRows - 1)
				fout << matrix[i][j] << "},\n";
			else if (j == numCols - 1 && i == numRows - 1)
				fout << matrix[i][j] << "}\n\t};\n";
			else
				fout << matrix[i][j] << ", ";
		}
	fout << "\tstd::string input;\n\n";
	fout << "\tstd::cout << \"Please input a string: \"; \n";
	fout << "\tgetline(std::cin, input);\n";
	fout << "\tstd::cout << std::endl;\n\n";
	fout << "\tfor(char& inputIter : input)\n\t{\n";
	fout << "\t\tif(state >= 10000)\n\t\t\tbreak;\n\n";
	fout << "\t\tchar *character = std::find(std::begin(alpha), std::end(alpha), inputIter);\n";
	fout << "\t\tif(character == std::end(alpha)) {\n";
	fout << "\t\t\tstate = 1000;\n\t\t\tbreak;\n\t\t}\n\n";
	fout << "\t\telse {\n";
	fout << "\t\t\tint symbolIndex = std::distance(alpha, std::find(std::begin(alpha), std::end(alpha), inputIter));\n";
	fout << "\t\t\tstate = matrix[state][symbolIndex];\n\t\t}\n\t}";
	fout << "\tswitch(state) {\n";
	//Begin output of error messages.
	for (auto &message : messages)
	{
		fout << "\t\t case " << message.first << ":\n";
		fout << "\t\t\tstd::cout << \"" << message.second << "\" << std::endl;\n\t\t\tbreak;\n";
	}
	fout << "\t\tdefault:\n\t\t\t std::cout << \"Invalid input symbol detected!\" << std::endl;\n\n\t}";
	fout << "\treturn 0;\n}";

	printf("Compiler successfully created!\n");

	return 0;

}