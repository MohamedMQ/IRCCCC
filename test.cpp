#include <iostream>
#include <vector>
using namespace std;
#include <ctime>

void age(int actual_day, int actual_mounth, int actual_year,int birthday, int birth_mounth, int birth_year)
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
	int actual_day;
	int actual_mounth;
	int actual_year;
	int birthday;
	int birthmounth;
	int birthyear;
	int pd, pm, py, bd, bm, by;
	std::string mounts[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	time_t now = time(0);
	char* dt = ctime(&now);
	char *str;
	std::vector<std::string> tokens;
	std::vector<std::string> tokens2;
	str = strtok(dt, " ");
	while (str != NULL)
	{
		tokens.push_back(str);
		str = strtok(NULL, " ");
	}
	actual_mounth = get_mounth(mounts, tokens[1]);
	actual_day = atoi(tokens[2].c_str());
	actual_year = atoi(tokens[4].c_str());
	str = NULL;
	str = strtok(argv[1], "-");
	while (str != NULL)
	{
		tokens2.push_back(str);
		str = strtok(NULL, "-");
	}
	birthday = atoi(tokens2[0].c_str());
	birthmounth = atoi(tokens2[1].c_str());
	birthyear = atoi(tokens2[2].c_str());
	age(actual_day, actual_mounth, actual_year, birthday, birthmounth, birthyear);
  return 0;
}