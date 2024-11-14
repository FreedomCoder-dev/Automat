#include "MealyMooreConverter.h"

const string MOORE_TO_MEALY_PARAM = "moore-to-mealy";
//функция печати полностью исправна
void PrintFile(vector<vector<string>>& TableResult, const string& filename)
{
    ofstream file(filename);

    for (const auto& row : TableResult) {
        for (size_t i = 0; i < row.size(); ++i) {
            const auto& cell = row[i];
            file << cell;
            if (i < row.size() - 1) {
                file << ";";
            }
        }
        file << std::endl;
    }
}
//функция полностью исправна из Мили->Мура
vector<vector<string>> ProcessMealy(vector<vector<string>>& TableMealy)
{
	vector<vector<string>> result;

    map<string, string> stateToOutput;
    unordered_set<string> uniqueOutputs;
    vector<string> orderedOutputs;

    for (size_t i = 1; i < TableMealy.size(); ++i) {
        for (size_t j = 1; j < TableMealy[i].size(); ++j) {
            string cell = TableMealy[i][j];
            size_t slashPos = cell.find('/');
            string state = cell.substr(0, slashPos);
            string output = cell.substr(slashPos + 1);
            stateToOutput[state] = output;
            if (uniqueOutputs.find(cell) == uniqueOutputs.end())
            {
                uniqueOutputs.insert(cell);
                orderedOutputs.push_back(cell);
            }
        }
    }

    //Создать новые состояния для автомата Мура
    map<string, string> newStateMap;
    int newStateIndex = 0;
    for (const auto& combined : orderedOutputs)
    {
        string newState = "q" + to_string(newStateIndex++);
        newStateMap[combined] = newState;
    }

    //Заполнить верхнюю часть таблицы Мура
    vector<string> headerRow = { "" }, headerOut = { "" };
    string first;
    for (const auto& combined : orderedOutputs) {
        string newState = newStateMap[combined];
        string output = combined.substr(combined.find('/') + 1);
        size_t slash = combined.find('/');
        first = combined.substr(0, slash);
        if (first == TableMealy[0][1])
        {
            headerRow.insert(headerRow.begin() + 1, newState);
            headerOut.insert(headerOut.begin() + 1, output);
        }
        else {
            headerRow.push_back(newState);
            headerOut.push_back(output);
        }
    }
    result.push_back(headerOut);
    result.push_back(headerRow);

    //Заполнить таблицу Мура на основе таблицы Мили
    unsigned int el = 0;
    for (size_t i = 1; i < TableMealy.size(); ++i) {
        vector<string> newRow = { TableMealy[i][0] };
        newRow.resize(result[1].size());
        for (size_t j = 1; j < TableMealy[i].size(); ++j) {
            string cell = TableMealy[i][j],
                newState = newStateMap[cell];
            size_t slashPos = cell.find('/');
            string output = TableMealy[0][j];//состояние из начальной таблицы

            for (const string& element : orderedOutputs) {
                cell = element;
                slashPos = cell.find('/');
                cell = cell.substr(0, slashPos);//состояние из полученной таблицы
                el++;
                if (output == cell)
                {
                    newRow[el] = newState;
                }
            }
            el=0;
        }
        result.push_back(newRow);
        el = 0;
    }

	return result;
}
//функция полностью исправна из Мили->Мура
void RemoveStatesMealy(vector<vector<string>>& TableMealy) {
    queue<string> stateQueue;
    set<string> reachableStates;
    string currentState;
    unsigned int i = 1;
    reachableStates.insert(TableMealy[1][1]);
    for (int l = 2; l < TableMealy.size(); l++)
    {
        stateQueue.push(TableMealy[l][1]);
    }
    while(!stateQueue.empty())
    {
        currentState = stateQueue.front();
        stateQueue.pop();
        i = 1;
        for (i; i < TableMealy[1].size(); i++)
        {
            if (currentState == TableMealy[1][i])
            {
                break;
            }
        }
        for (int l = 1; l < TableMealy[1][i].size(); l++)
        {
            if (reachableStates.find(currentState) == reachableStates.end())
            {
                reachableStates.insert(currentState);
                for (int k = 2; k < TableMealy.size(); k++) 
                {
                    if (currentState != TableMealy[k][i])
                    {
                        stateQueue.push(TableMealy[k][i]);
                    }
                }
            }
        }
    }
    for (int l = 1; l < 2; l++)
    {
        for (int k = 1; k < TableMealy[0].size(); k++)
        {
            currentState = TableMealy[l][k];
            if (reachableStates.find(currentState) == reachableStates.end())
            {
                for (int m = 0; m < TableMealy.size(); m++)
                {
                    TableMealy[m].erase(TableMealy[m].begin() + k);
                }
            }
        }
    }
}
//функция полностью исправна из Мура->Мили
vector<vector<string>> ProcessMoore(vector<vector<string>>& TableMoore, string newStatePrefix = "S")
{
    map<string, string> stat;
    map<string, string> newStat;
    string state, first;
    for (size_t i = 1; i < TableMoore[0].size(); ++i)
    {
        for (size_t j = 2; j < TableMoore.size(); ++j)
        {
            state += TableMoore[j][i];
        }
        first = TableMoore[1][i] + "/" + TableMoore[0][i];
        stat[state] += first + ",";
        state = "";
    }
    unsigned int el = 0;
    for (auto it = stat.begin(); it != stat.end(); ++it)
    {
        newStat[it->second] = "S" + to_string(el);
        el++;
    }
    vector<vector<string>> result;
    vector<string> row;
    
    row.push_back("");
    for (size_t i = 1; i < TableMoore[0].size(); ++i)
    {
               
        for (size_t j = 0; j < 1; ++j)
        {
            first = TableMoore[1][i] + "/" + TableMoore[0][i];
            for (auto it = stat.begin(); it != stat.end(); ++it)
            {
                state = it->second;
                size_t pos = state.find(first);
                if(pos != std::string::npos)
                {
                    
                    row.push_back(newStat[state]);
                    break;
                }
            }
        }
    }
    result.push_back(row);
    string second;
    for (size_t i = 2; i < TableMoore.size(); ++i)
    {
        row = { TableMoore[i][0] };
        for (size_t j = 1; j < TableMoore[i].size(); ++j)
        {
            first = TableMoore[i][j] + "/";
            for (auto it = newStat.begin(); it != newStat.end(); ++it)
            {
                state = it->first;
                size_t pos = state.find(first);
                if (pos != std::string::npos)
                {
                    size_t slash = state.find('/', pos);
                    size_t point = state.find(',', slash);
                    second = state.substr(slash + 1, point - slash - 1);
                    state = newStat[state] + "/" + second;
                    row.push_back(state);
                    break;
                }
            }
        }
        result.push_back(row);
    }
    return result;
}
//функция полностью исправна из Мура->Мили
void RemoveStatesMoore(vector<vector<string>>& TableMoore) 
{
    set<string> state;
    string first, second;
    for (size_t i = 1; i < TableMoore.size(); ++i)
    {
        for (size_t j = 1; j < TableMoore[i].size(); ++j)
        {
            first = TableMoore[i][j];
            size_t slash = first.find('/');
            first = first.substr(0, slash);
            state.insert(first);
        }
    }
    unsigned int el = 1;
    queue<string> stateQueue;
    for (size_t i = 1; i < TableMoore[0].size(); ++i)
    {
        first = TableMoore[0][i];
        if (state.find(first) == state.end())
        {
            for (size_t l = 0; l < TableMoore.size(); ++l)
            {
                if (l != 0)
                {
                    second = TableMoore[l][i];
                    size_t slash = second.find('/');
                    second = second.substr(0, slash);
                    stateQueue.push(second);
                }
                TableMoore[l].erase(TableMoore[l].begin() + i);
            }
            i--;
        }
    }
    while (!stateQueue.empty())
    {
        first = stateQueue.front();
        for (size_t i = 1; i < TableMoore[0].size(); ++i)
        {
            if (stateQueue.empty()) break;
            for (size_t l = 1; l < TableMoore.size(); ++l)
            {
                second = TableMoore[l][i];
                size_t slash = second.find('/');
                second = second.substr(0, slash);
                if (second == first)
                {
                    stateQueue.pop();
                    break;
                }
            }
            if (i == TableMoore[0].size() - 1)
            {
                for (size_t k = 1; k < TableMoore[0].size(); ++k)
                {
                    second = TableMoore[0][k];
                    if (second == first)
                    {
                        for (size_t m = 0; m < TableMoore.size(); ++m)
                        {
                            TableMoore[m].erase(TableMoore[m].begin() + k);
                        }
                        break;
                    }
                }
                stateQueue.pop();
            }
        }
    }
}
//функция полностью исправна из Мура->Мили
void DeleteStatesMoore(vector<vector<string>>& TableMoore)
{
    set<string> state;
    string first, second;
    for (size_t i = 1; i < TableMoore[0].size(); ++i)
    {
        first = TableMoore[0][i];
        if (state.find(first) == state.end())
        {
            state.insert(first);
        }
        else {
            for (size_t m = 0; m < TableMoore.size(); ++m)
            {
                TableMoore[m].erase(TableMoore[m].begin() + i);
            }
            i--;
        }
    }
}

int main(int argc, char* argv[])
{
    string workParam = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];

	ifstream file(inputFile);
	vector<vector<string>> table;
	string line;

    if (workParam == MOORE_TO_MEALY_PARAM)//Это из Мура в Мили
    {
	    if (getline(file, line)) {
		    vector<string> row;
		    stringstream ss(line);
		    string cell;

		    while (getline(ss, cell, ';')) {
			    row.push_back(cell);
		    }
		    table.push_back(row);
	    }

        if (getline(file, line)) {
            vector<string> row;
            stringstream ss(line);
            string cell;

            while (getline(ss, cell, ';')) {
                row.push_back(cell);
            }
            table.push_back(row);
        }

        while (getline(file, line)) {
            vector<string> row;
            stringstream ss(line);
            string cell;

            while (getline(ss, cell, ';')) {
                row.push_back(cell);
            }
            table.push_back(row);
        }
        file.close();
	    vector<vector<string>> resultTable = ProcessMoore(table);
        RemoveStatesMoore(resultTable);
	DeleteStatesMoore(resultTable);
        PrintFile(resultTable, outputFile);
    }
    else//Это из Мили в Мура
    {
	    while (getline(file, line))
	    {
		    vector<string> row;
		    stringstream ss(line);
		    string cell;

		    while (getline(ss, cell, ';'))
		    {
			    row.push_back(cell);
		    }
		    table.push_back(row);
	    }
        file.close();
        vector<vector<string>> resultTable = ProcessMealy(table);
        RemoveStatesMealy(resultTable);
        PrintFile(resultTable, outputFile);
    }

	return 0;
}
