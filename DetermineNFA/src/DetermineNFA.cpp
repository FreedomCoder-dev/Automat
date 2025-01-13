#include "DetermineNFA.h"

struct State {
    bool IsFinit = false;
    string StateName;
    map<string, vector<string>> Transitions;
};

struct Epsilon {
    string StateName;
    vector<string> TransitionsName;
};

vector<State> ReadFromCSV(const string& inputFileName) {
    ifstream file(inputFileName);
    vector<State> states;
    string line;

    // Чтение первой строки (конечные состояния)
    getline(file, line);
    vector<string> finalStates;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ';')) {
        finalStates.push_back(token);
    }
    if (token == "")
    {
        finalStates.push_back(token);
    }

    // Чтение второй строки (имена состояний)
    getline(file, line);
    vector<string> stateNames;
    stringstream ss2(line);
    while (getline(ss2, token, ';')) {
        if (!token.empty()) {
            stateNames.push_back(token);
        }
    }

    // Создание состояний
    for (size_t i = 0; i < stateNames.size(); ++i) {
        State state;
        state.StateName = stateNames[i];
        state.IsFinit = !finalStates[i + 1].empty();
        states.push_back(state);
    }

    // Чтение оставшихся строк (переходы)
    while (getline(file, line)) {
        stringstream ss3(line);
        vector<string> parts;
        while (getline(ss3, token, ';')) {
            parts.push_back(token);
        }
        if (token == "")
        {
            parts.push_back("");
        }
        string transitionName = parts[0];

        for (size_t j = 1; j < parts.size(); ++j) {
            if (!parts[j].empty()) {
                stringstream ss4(parts[j]);
                vector<string> transitions;
                while (getline(ss4, token, ',')) {
                    if (!token.empty()) {
                        transitions.push_back(token);
                    }
                }
                states[j - 1].Transitions[transitionName] = transitions;
            }
            else {
                states[j - 1].Transitions[transitionName] = vector<string>();
            }
        }
    }

    return states;
}

vector<Epsilon> FindEpsilonStates(const vector<State>& inputStates) {
    vector<Epsilon> epsilonStates;

    for (const auto& state : inputStates) {
        Epsilon tempEpsilonState;
        tempEpsilonState.StateName = state.StateName;

        unordered_set<string> statesList = { state.StateName };
        queue<string> queue;
        queue.push(state.StateName);

        while (!queue.empty()) {
            string currentStateName = queue.front();
            queue.pop();

            auto currentState = find_if(inputStates.begin(), inputStates.end(), [&](const State& s) {
                return s.StateName == currentStateName;
                });

            if (currentState->Transitions.count("Оµ")) {// currentState != inputStates.end() &&
                for (const auto& transition : currentState->Transitions.at("Оµ")) {
                    if (statesList.find(transition) == statesList.end()) {
                        statesList.insert(transition);
                        queue.push(transition);
                    }
                }
            }
        }

        tempEpsilonState.TransitionsName.assign(statesList.begin(), statesList.end());
        epsilonStates.push_back(tempEpsilonState);
    }

    return epsilonStates;
}

unordered_set<string> GetDependencies(const string& newState, const map<string, vector<string>>& statesToIterate, const vector<Epsilon>& statesWithEpsilon) {
    unordered_set<string> dependency;

    for (const auto& state : statesToIterate.at(newState)) {
        for (const auto& epsilonState : statesWithEpsilon) {
            if (state == epsilonState.StateName) {
                for (const auto& transition : epsilonState.TransitionsName) {
                    dependency.insert(transition);
                }
            }
        }
    }

    return dependency;
}

string FindNewStateKey(const map<string, vector<string>>& iterateStates, const vector<string>& transitionsToCheck) {
    unordered_set<string> transitionsSet(transitionsToCheck.begin(), transitionsToCheck.end());

    for (const auto& [key, value] : iterateStates) {
        unordered_set<string> valueSet(value.begin(), value.end());
        if (valueSet == transitionsSet) {
            return key;
        }
    }

    return "";
}

vector<State> NewStates(const vector<State>& inputStates, const vector<Epsilon>& statesWithEpsilon) {
    vector<State> newStates;
    map<string, vector<string>> statesToIterate;
    int stateCounter = 0;

    // Инициализация начального состояния
    statesToIterate["s0"] = { inputStates[0].StateName };
    vector<string> states = { "s0" };

    // Основной перебор
    for (size_t i = 0; i < statesToIterate.size(); ++i) {
        string newState = states[i];
        unordered_set<string> dependency = GetDependencies(newState, statesToIterate, statesWithEpsilon);

        bool isFinit = any_of(dependency.begin(), dependency.end(), [&](const string& t) {
            return any_of(inputStates.begin(), inputStates.end(), [&](const State& s) {
                return s.StateName == t && s.IsFinit;
                });
            });

        State tempNewState;
        tempNewState.StateName = newState;
        tempNewState.IsFinit = isFinit;

        for (const auto& symbol : inputStates[0].Transitions) {
            if (symbol.first != "Оµ") {
                vector<string> transitions;
                for (const auto& iState : inputStates) {
                    if (any_of(dependency.begin(), dependency.end(), [&](const string& s) {
                        return s == iState.StateName;
                        })) {
                        if (iState.Transitions.count(symbol.first)) {
                            transitions.insert(transitions.end(), iState.Transitions.at(symbol.first).begin(), iState.Transitions.at(symbol.first).end());
                        }
                    }
                }

                sort(transitions.begin(), transitions.end());
                transitions.erase(unique(transitions.begin(), transitions.end()), transitions.end());

                string newStateKey = "";
                if (!transitions.empty()) {
                    newStateKey = FindNewStateKey(statesToIterate, transitions);
                }

                if (newStateKey.empty() && !transitions.empty()) {
                    stateCounter++;
                    newStateKey = "s" + to_string(stateCounter);

                    statesToIterate[newStateKey] = transitions;
                    states.push_back(newStateKey);
                }

                tempNewState.Transitions[symbol.first] = { newStateKey };
            }
        }

        newStates.push_back(tempNewState);
    }

    return newStates;
}

void WriteToFile(const string& outputFileName, const vector<State>& outputStates) {
    ofstream writer(outputFileName);

    // Заголовки для конечных состояний и имен состояний
    writer << ";";
    for (const auto& state : outputStates) {
        writer << (state.IsFinit ? "F" : "") << ";";
    }
    writer << endl;

    writer << ";";
    for (const auto& state : outputStates) {
        writer << state.StateName << ";";
    }
    writer << endl;

    // Создаем строки для переходов
    vector<vector<string>> rows;
    vector<string> symbols;
    for (const auto& symbol : outputStates[0].Transitions) {
        symbols.push_back(symbol.first);
    }

    for (const auto& symbol : symbols) {
        vector<string> row = { symbol };

        for (const auto& state : outputStates) {
            if (state.Transitions.count(symbol)) {
                string transitions;
                for (const auto& nextState : state.Transitions.at(symbol)) {
                    transitions += nextState + ",";
                }
                if (!transitions.empty()) {
                    transitions.pop_back(); // Убираем последнюю запятую
                }
                row.push_back(transitions);
            }
            else {
                row.push_back("");
            }
        }

        rows.push_back(row);
    }

    // Записываем строки переходов в файл
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            writer << row[i];
            if (i < row.size() - 1) {
                writer << ";";
            }
        }
        writer << endl;
    }
}

int main(int argc, char* argv[]) {
	string inputFile = argv[1];
	string outputFile = argv[2];

	vector<State> inputStates = ReadFromCSV(inputFile);//полностью правильная
	vector<Epsilon> statesWithEpsilon = FindEpsilonStates(inputStates);
	vector<State> outputStates = NewStates(inputStates, statesWithEpsilon);

	WriteToFile(outputFile, outputStates);
	
	return 0;
}
