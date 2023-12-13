#include <iostream>
#include <vector>
using namespace std;
#include <ctime>

void count_age(int actual_day, int actual_mounth, int actual_year,int birthday, int birth_mounth, int birth_year)
{
	int day;
	int mounth;
	int year;
	int md[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	year = actual_year - birth_year;
	if (actual_mounth < birth_mounth)
	{
		year--;
		mounth = 12 - (birth_mounth - actual_mounth);
	}
	else
		mounth = actual_mounth - birth_mounth;
	if (actual_day < birthday)
	{
		mounth--;
		day = md[actual_mounth - 1] - (birthday - actual_day);
	}
	else
	day = actual_day - birthday;
	std::cout << "your age is : ";
	std::cout << year << " years " << mounth << " months " << day << " days. ";
}

int get_mounth(std::string mounths[], std::string mounth)
{
	for (int i = 0; i < 12; i++)
	{
		if (mounths[i] == mounth)
			return i + 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
		time_t now = time(0);
		char* dt = ctime(&now);
	std::cout << dt;
  return 0;
}