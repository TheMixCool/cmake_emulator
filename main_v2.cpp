#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

struct target {
	std::vector<std::string> actions;
	std::set<std::string> depends;
	bool compileStatus;
	int compilatedCount = 0;
};


enum {
	NOT_COMPILED,
	COMPILED
};


bool check_argument_count(const int& count) {

	if (count > 3) {
		std::cout << "Too many arguments\nExit...";
		return 0;
	}

	if (count < 3) {
		std::cout << "Not enough arguments\nExit...";
		return 0;
	}
	return 1;
}


void print(std::vector<std::string>& queueTarget, std::map<std::string, target>& targets) {

	for (const auto& element : queueTarget) {
		std::cout << element << std::endl;

		for (const auto& action : targets[element].actions) {
			std::cout << action << std::endl;
		}
	}
}


std::string create_target(const std::string& str, std::map<std::string, target>& targets) {

	size_t endTargetPosition = str.find(':');

	std::string token;
	std::string newTarget = str.substr(0, endTargetPosition);

	std::istringstream strToDepends(str.substr(endTargetPosition + 1));

	while (getline(strToDepends, token, ' '))
		if (!token.empty())
			targets[newTarget].depends.emplace(token);

	
	targets[newTarget].compileStatus = NOT_COMPILED;

	return newTarget;
}


void file_data_to_map(std::ifstream& file, std::map<std::string, target>& targets) {

	std::string str;
	std::string targetName;

	while (!file.eof()) {
		std::getline(file, str);

		if (str.empty())
			continue;

		if (str[0] == ' ' || str[0] == '\t')
			targets[targetName].actions.push_back(str);
		else
			targetName = create_target(str, targets);
	}
}


bool check_argument_target(const std::string& arg, const std::map<std::string, target>& targets) {

	if (targets.find(arg) != targets.end())
		return 1;

	std::cout << "There is no work: " << arg << '\n';
	return 0;
}


void compile(std::map<std::string, target> &targets, const std::string& workName, int& currentWorks, int& maxWorks, std::vector<std::string> &order) {
	while (currentWorks != maxWorks) {

		std::string temp = workName;

		if (targets[temp].compileStatus == NOT_COMPILED) {
			for (auto& el : targets[temp].depends) {
				if (targets[el].compileStatus == NOT_COMPILED) {
					currentWorks++;
					compile(targets, el, currentWorks, maxWorks, order);
				}
				if (currentWorks >= maxWorks)
					break;
			}
			targets[temp].compileStatus = COMPILED;
		}
		else {
			if (currentWorks >= maxWorks)
				break;

			targets[workName].compileStatus = COMPILED;

			order.push_back(workName);

			break;
		}
	}
}


int main(int argc, const char* argv[]) {

	if (!check_argument_count(argc))
		return 1;

	std::ifstream file;

	file.open(argv[1]);

	if (!file.is_open()) {
		std::cout << "File was not found\n";
		return 1;
	}

	std::map<std::string, target> targets;

	file_data_to_map(file, targets);

	if (!check_argument_target(argv[2], targets))
		return 1;

	std::vector<std::string> orderToCompile;
	orderToCompile.reserve(targets.size());
	
	int currentWorks = 0;
	int maxWorks = targets.size();

	compile(targets, argv[2], currentWorks, maxWorks, orderToCompile);

	if (currentWorks >= maxWorks) {
		std::cout << "Program could not be compliled\n" << std::endl;
		return 1;
	}
	
	print(orderToCompile, targets);

	return 0;
}