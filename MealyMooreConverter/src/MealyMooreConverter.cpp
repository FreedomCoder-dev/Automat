#include "MealyMooreConverter.h"

const string MOORE_TO_MEALY_PARAM = "moore-to-mealy";
//ôóíêöèÿ ïå÷àòè ïîëíîñòüþ èñïðàâíà
void PrintFile(vector<vector<string>>& TableResult, const string& filename)
{
    ofstream file(filename);

    for (const auto& row : TableResult) {
        for (const auto& cell : row) {
            file << cell << ";";
        }
        file << endl;
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
    for (const auto& combined : orderedOutputs) {
        string newState = newStateMap[combined];
        string output = combined.substr(combined.find('/') + 1);
        headerRow.push_back(newState);
        headerOut.push_back(output);
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

    for (size_t i = 1; i < TableMealy[0].size(); ++i)
    {
        for (size_t j = 2; j < TableMealy.size(); ++j)
        {
            reachableStates.insert(TableMealy[j][i]);
        }
    }
    for (size_t i = 1; i < 2; ++i)//было 0
    {
        for (size_t j = 1; j < TableMealy[i].size(); ++j)//было 1
        {
            currentState = TableMealy[i][j];
            //size_t slash = currentState.find('/');
            //currentState = currentState.substr(0, slash);
            if (reachableStates.find(currentState) == reachableStates.end())
            {
                for (size_t l = 0; l < TableMealy.size(); ++l)
                {
                    if ((l != 1) && (l != 0))
                    {
                        stateQueue.push(TableMealy[l][j]);
                    }
                    TableMealy[l].erase(TableMealy[l].begin() + j);
                }
                j--;
            }
        }
    }
    string norm;
    for (size_t i = 1; i < 2; ++i)//было 0 < 1
    {
        
        for (size_t j = 1; j < TableMealy[i].size(); ++j)
        {
            if(stateQueue.empty()) break;
            currentState = stateQueue.front();
            for (size_t l = 2; l < TableMealy.size(); ++l)//было 1
            {
                if (currentState == TableMealy[l][j])
                {
                    stateQueue.pop();
                    j = 0;
                }
            }
            if (j == (TableMealy[i].size() - 1))
            {
                for (size_t k = 1; k < TableMealy[i].size(); ++k)
                {
                    norm = TableMealy[i][k];
                    //size_t slash = norm.find('/');
                    //norm = norm.substr(0, slash);
                    if (norm == currentState)
                    {
                        for (size_t m = 0; m < TableMealy.size(); ++m)
                        {
                            TableMealy[m].erase(TableMealy[m].begin() + k);
                        }
                        break;
                    }
                }
                stateQueue.pop();
                j = 0;
            }
        }
    }
}

vector<vector<string>> ProcessMoore(vector<vector<string>>& TableMoore, string newStatePrefix = "S")
{
    vector<vector<string>> result;
    vector<string> states = TableMoore[1];
    string first, second;
    unsigned int count = 0, i = 2;
    map<string, string> stat;
    


    for (size_t j = 1; j < TableMoore[i].size(); ++j)
    {
        for (int l = 2; l < TableMoore.size(); l++)
        {
            first += TableMoore[l][j];
        }
        
        if (TableMoore[i].size() == j+1)
        {
            second = "";
        }
        else {
            for (int l = 2; l < TableMoore.size(); l++)
            {
                second += TableMoore[l][j + 1];
            }
        }
           
        if (first == second)
        {
            stat[states[j]] = newStatePrefix + to_string(count);
        }
        else
        {
            stat[states[j]] = newStatePrefix + to_string(count);
            count++;
        }
        first = "";
        second = "";
    }
    states.clear();

    vector<string> row;
    row.push_back("");
    for (const auto& pair : stat)
    {
        row.push_back(pair.second);
    }
    result.push_back(row);
    row.clear();

    for (size_t i = 2; i < TableMoore.size(); ++i) {
        vector<string> newRow;
        newRow.push_back(TableMoore[i][0]);

        for (size_t j = 1; j < TableMoore[i].size(); ++j) {
            string state = TableMoore[i][j], output;

            for (size_t l = 1; l < TableMoore[i].size(); ++l)
            {
                if (state == TableMoore[1][l])
                {
                    output = TableMoore[0][l];
                    break;
                }
            }
            if (stat.find(state) != stat.end()) {
                newRow.push_back(stat[state] + "/" + output);
            }
            else {
                newRow.push_back(state + "/" + output);
            }
        }

        result.push_back(newRow);
    }

    return result;
}

void RemoveStatesMoore(vector<vector<string>>& TableMoore) {

    string initialState = TableMoore[0][1];

    unordered_set<string> reachableStates;
    queue<string> stateQueue;
    stateQueue.push(initialState);
    reachableStates.insert(initialState);

    for (size_t j = 1; j < TableMoore.size(); ++j)
    {
        string nextState = TableMoore[j][1];
        nextState = nextState.substr(0, 2);        
        reachableStates.insert(nextState);
    }
    
    unsigned int count = 2, i = 1, j = 2;
    while (j != TableMoore[i].size())
    {
        initialState = TableMoore[0][count];
        if (reachableStates.find(initialState) != reachableStates.end())
        {
            for (int k = 1; k < TableMoore.size(); k++)
            {
                initialState = TableMoore[k][j];
                initialState = initialState.substr(0, 2);
                if (reachableStates.find(initialState) != reachableStates.end())
                {
                }
                else
                {
                    reachableStates.insert(initialState);
                }
            }
            j++;
            count++;
        }
        else 
        {
            //удаляю колонку
            for (size_t l = 0; l < TableMoore.size(); l++)
            {
                TableMoore[l].erase(TableMoore[l].begin() + j);
            }
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
