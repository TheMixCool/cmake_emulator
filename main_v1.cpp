#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <stack>

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

	if (targets[newTarget].depends.empty())
		targets[newTarget].compileStatus = COMPILED;
	else
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


void compile_element(std::vector<std::string>& order, const std::string currentTarget, std::map<std::string, target>& targets, std::stack<std::string>& inStack, std::set<std::string>& isCompiled) {

	order.push_back(currentTarget);

	for (auto& [k, v] : targets)
		if (v.depends.find(currentTarget) != v.depends.end())
			v.compilatedCount++;

	isCompiled.emplace(currentTarget);
	inStack.pop();
}


bool compile(std::map<std::string, target>& targets, const std::string& targetToCompile, std::vector<std::string>& order) {

	std::stack<std::string> inStack;
	std::set<std::string> isCompiled;

	unsigned int currentStackSize = inStack.size();
	const unsigned int maxStackSize = targets.size();

	inStack.push(targetToCompile);

	while (currentStackSize <= maxStackSize && !inStack.empty()) {

		std::string currentTarget = inStack.top();

		if (targets[currentTarget].compileStatus == COMPILED && isCompiled.find(currentTarget) == isCompiled.end()) {

			compile_element(order, currentTarget, targets, inStack, isCompiled);

		}
		else if (targets[currentTarget].depends.size() == targets[currentTarget].compilatedCount && targets[currentTarget].compileStatus == NOT_COMPILED && isCompiled.find(currentTarget) == isCompiled.end()) {

			targets[currentTarget].compileStatus == COMPILED;
			compile_element(order, currentTarget, targets, inStack, isCompiled);
		}

		else {

			for (auto& element : targets[currentTarget].depends) {
				if (isCompiled.find(element) == isCompiled.end()) {
					inStack.push(element);
					currentStackSize++;
				}
			}
		}
	}

	if (currentStackSize > maxStackSize) {
		std::cout << "Program can not be compiled\nExit...";
		return 0;
	}

	return 1;
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

	if (!compile(targets, argv[2], orderToCompile))
		return 1;

	print(orderToCompile, targets);

	return 0;
}