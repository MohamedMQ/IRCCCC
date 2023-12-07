#include <iostream>
#include <vector>


void split_options(std::string to_split)
{
	std::vector<std::string> options;
	std::string tok;
	char sign = to_split[0];
	for (int i = 1; i < to_split.size(); i++)
	{
		tok = "";
		tok += sign;
		tok+= to_split[i];
		options.push_back(tok);
	}
	for (int i = 0; i < options.size() ; i++)
	{
		std::cout << "[" << options[i] << "]" << std::endl;
	}
}

int main()
{
	split_options("+oprtlk");
}