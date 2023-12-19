#include <vector>
#include <iostream>

int pars_ip(std::string str)
{
	char *str2;
	std::vector<char *> tokens;
	str2 = strtok((char *)str.c_str(), ".");
	while (str2 != NULL)
	{
		tokens.push_back(str2);
		str2 = strtok(NULL, ".");
	}
	std::cout << tokens.size();
	return 1;
}

int main()
{
	int i = pars_ip("127.1.0");
}