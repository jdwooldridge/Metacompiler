#include <iostream>
#include <string>
#include <algorithm>
int main()
{
	int state = 0;
	char alpha[3] = {'a', 'b', 'c' };
	int matrix[2][3] =
	{
		{1, 10001, 10002},
		{10003, 1, 1}
	};
	std::string input;

	std::cout << "Please input a string: "; 
	getline(std::cin, input);
	std::cout << std::endl;

	for(char& inputIter : input)
	{
		if(state >= 10000)
			break;

		char *character = std::find(std::begin(alpha), std::end(alpha), inputIter);
		if(character == std::end(alpha)) {
			state = 1000;
			break;
		}

		else {
			int symbolIndex = std::distance(alpha, std::find(std::begin(alpha), std::end(alpha), inputIter));
			state = matrix[state][symbolIndex];
		}
	}	switch(state) {
		 case 0:
			std::cout << "Empty string is not allowed!" << std::endl;
			break;
		 case 1:
			std::cout << "VALID" << std::endl;
			break;
		 case 10001:
			std::cout << "String cannot begin with 'b'!" << std::endl;
			break;
		 case 10002:
			std::cout << "String cannot begin with 'c'!" << std::endl;
			break;
		 case 10003:
			std::cout << "'a' is illegal here!" << std::endl;
			break;
		default:
			 std::cout << "Invalid input symbol detected!" << std::endl;

	}	return 0;
}