#include "Minimize.h"

const string MOORE_TO_MEALY_PARAM = "mealy";
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

//Ýòî Ìèëè
void RemoveStatesMealy(vector<vector<string>>& TableMealy)
{
    queue<string> stateQueue;
    set<string> reachableStates;
    set<string> markState;//вектор стейтов, в которых уже были
    string currentState;
    reachableStates.insert(TableMealy[0][1]);
    for (int l = 1; l < TableMealy.size(); l++)
    {
        currentState = TableMealy[l][1];
        currentState = currentState.substr(0, currentState.find('/'));
        if (markState.find(currentState) == markState.end())
        {
            markState.insert(currentState);
            stateQueue.push(currentState);
        }
        
    }
    while (!stateQueue.empty())
    {
        currentState = stateQueue.front();
        stateQueue.pop();
        if (reachableStates.find(currentState) == reachableStates.end())
        {
            reachableStates.insert(currentState);
            for (int n = 1; n < TableMealy[0].size(); n++)
            {
                if (currentState == TableMealy[0][n])
                {
                    for (int i = 1; i<TableMealy.size(); i++)
                    {
                        string mark = TableMealy[i][n];
                        mark = mark.substr(0, mark.find('/'));
                        if (markState.find(mark) == markState.end())
                        {
                            markState.insert(mark);
                            stateQueue.push(mark);
                        }
                    }
                }
            }
        }
    }
    for (int l = 0; l < 1; l++)
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
                k--;
            }
        }
    }
}

void GetStateMealyOne(map<string, string>& State, vector<vector<string>>& Mealy, string& newState)
{
    set<string> Signal;
    map<string, string> FreeSignal;
    unsigned int count = 1;
    for (int i = 1; i < Mealy[0].size(); i++)
    {
        string states = "", record = "";
        for (int l = 1; l < Mealy.size(); l++)
        {
            states = Mealy[l][i];
            states = states.substr(states.find('/') + 1);
            record += states + ',';
        }
        if (Signal.find(record) == Signal.end())
        {
            Signal.insert(record);
            FreeSignal[Mealy[0][i]] = record;
            State[Mealy[0][i]] = newState + to_string(count);
            count++;
        }
        else
        {
            for (auto& combin : FreeSignal)
            {
                if (record == combin.second)
                {
                    for (auto& com : State)
                    {
                        if (combin.first == com.first)
                        {
                            State[Mealy[0][i]] = com.second;
                            break;
                        }
                    }
                }
            }
        }
    }
    newState[0] = newState[0] + 1;
}

vector<vector<string>> FillTheTableMealyOne(map<string, string>& State, vector<vector<string>>& Mealy)
{
    vector<vector<string>> result;
    vector<string> states = { "" }, mealyState = {""};
    for (int k = 1; k < Mealy[0].size(); k++)
    {
        string symbol = Mealy[0][k];
        for (auto& combin : State)
        {
            string nextState = combin.first;
            if(nextState == symbol)
            {
                mealyState.push_back(combin.first);
                states.push_back(combin.second);
                break;
            }
        }
    }

    result.push_back(states);
    result.push_back(mealyState);

    for (int i = 1; i < Mealy.size(); i++)
    {
        states = { Mealy[i][0]};
        for (int l = 1; l < Mealy[0].size(); l++)
        {
            string nextStates = Mealy[i][l];
            nextStates = nextStates.substr(0, nextStates.find('/'));
            for (auto& combin : State)
            {
                string fillState = combin.first;
                fillState = fillState.substr(0, fillState.find('/'));
                if ((fillState == nextStates) || (Mealy[0].size() == 2))
                {
                    states.push_back(combin.second);
                    break;
                }
            }
        }
        result.push_back(states);
    }
    return result;
}

void GetStateMealy(map<string, string>& State, vector<vector<string>>& Mealy, string& newState)
{
    set<string> Signal;
    unsigned int count = 1;
    for (int l = 1; l < Mealy[0].size(); l++)
    {
        string states = "", record = "";
        for (int i = 2; i < Mealy.size(); i++)
        {
            states = Mealy[i][l];
            record += states + ',';
        }
        if (Signal.find(record) == Signal.end())
        {
            Signal.insert(record);
            State[Mealy[0][l] + "/" + record] = newState + to_string(count);
            count++;
        }
        else {
            for (auto& combin : State)
            {
                string newState = combin.first;
                newState = newState.substr(newState.find('/') + 1);
                if (newState == record)
                {
                    State[Mealy[0][l] + "/" + record] = combin.second;
                    break;
                }
            }
        }
    }
    newState[0] = newState[0] + 1;
}

vector<vector<string>> FillTheTableMealy(map<string, string>& State, vector<vector<string>>& Mealy)
{
    vector<vector<string>> result;
    vector<string> states = { "" }, mealyState = { "" };
    for (int k = 1; k < Mealy[0].size(); k++)
    {
        for (auto& combin : State)
        {
            string symbol = "";
            for (int m = 2; m < Mealy.size(); m++)
            {
                if (m == 1) continue;
                symbol += Mealy[m][k] + ",";
            }
            string rel = combin.first;
            rel = rel.substr(combin.first.find('/') + 1);
            if (symbol == rel)
            {
                mealyState.push_back(Mealy[1][k]);
                //states.push_back(combin.first);
                states.push_back(combin.second);
                break;
            }
        }
    }

    result.push_back(states);
    result.push_back(mealyState);

    for (int i = 2; i < Mealy.size(); i++)
    {
        states = { Mealy[i][0] };
        for (int l = 1; l < Mealy[0].size(); l++)
        {
            string nextStates = Mealy[i][l];
            for (auto& combin : State)
            {
                string fillState = combin.first;
                fillState = fillState.substr(0, fillState.find('/'));
                if ((fillState == nextStates) || (Mealy[0].size() == 2))
                {
                    states.push_back(combin.second);
                    break;
                }
            }
        }
        result.push_back(states);
    }
    return result;
}

void DeleteStatesMealy(vector<vector<string>>& result, unsigned int& countState, vector<vector<string>>& Mealy)
{
    map<string, string> newState;
    set<string> signal;
    set<string> signalState;
    map<string, string> State;
    for (int i = 1; i < result[0].size(); i++)
    {
        string nightState = "";
        for (int l = 0; l < result.size(); l++)
        {
            if (l == 1)
            {
                State[nightState] += result[l][i] + ",";
                signalState.insert(result[l][i]);
                continue;
            }
            string rel = result[l][i];
            nightState += rel;
        }
        if (signal.find(nightState) == signal.end())
        {
            signal.insert(nightState);
        }
        else
        {
            for (int k = 0; k < result.size(); k++)
            {
                if (signalState.find(result[k][i]) != signalState.end())
                {
                    signalState.erase(result[k][i]);
                }
                result[k].erase(result[k].begin() + i);
            }
            i--;
        }
    }
    vector<vector<string>> newResultMealy;
    vector<string> fillResultMealy;

    newResultMealy.push_back(result[0]);
    for (int i = 1; i < Mealy.size(); i++)
    {
        fillResultMealy = { Mealy[i][0] };   
        for (int l = 1; l < result[0].size(); l++)
        {
            string signalStr = Mealy[i][l];
            signalStr = signalStr.substr(signalStr.find('/') + 1);
            if (signalState.find(Mealy[0][l]) != signalState.end())
            {
                for (auto& combin : State)
                {
                    string rel = Mealy[i][l];
                    rel = rel.substr(0, rel.find('/'));
                    string cel = combin.second;
                    if (cel.find(rel) != std::string::npos)
                    {
                        fillResultMealy.push_back(combin.first + "/" + signalStr);
                        break;
                    }
                }
            }
        }
        newResultMealy.push_back(fillResultMealy);
    }
    Mealy = newResultMealy;
}

void MinimizeMealy(vector<vector<string>>& TableMealy)
{
    map<string, string> State;
    string newState = "A";
    GetStateMealyOne(State, TableMealy, newState);
    vector<vector<string>> result = FillTheTableMealyOne(State, TableMealy);
    unsigned int countState = State.size();
    bool flag = true;
    while (flag)
    {
        State.clear();
        GetStateMealy(State, result, newState);
        result = FillTheTableMealy(State, result);
        if (State.size() == countState)
        {
            flag = false;
        }
        countState = State.size();
    }
    DeleteStatesMealy(result, countState, TableMealy);//доделать функцию
}

//Ýòî Ìóð
void GetStateMoore(map<string, string>& State, vector<vector<string>>& Moore, string& newState)
{
    set<string> Signal;
    unsigned int count = 1;
    for (int l = 1; l < Moore[0].size(); l++)
    {
        string states = Moore[1][l] + "/" + Moore[0][l];
        string newSignal = "";
        if (Signal.find(Moore[0][l]) == Signal.end())
        {
            Signal.insert(Moore[0][l]);
            State[states] = newState + to_string(count);
            count++;
        }
        else
        {
            for (auto& combin : State)
            {
                newSignal = combin.first;
                newSignal = newSignal.substr(newSignal.find('/') + 1);
                string record = states.substr(states.find('/') + 1);
                if (newSignal == record)
                {
                    State[states] = combin.second;
                    break;
                }
            }
        }
    }
    newState[0] = newState[0] + 1;
}

vector<vector<string>> FillTheTableMoore(map<string, string>& State, vector<vector<string>>& Moore)
{
    vector<vector<string>> result;
    string startState = Moore[1][1];
    vector<string> signal = { "" }, headState = { "" };
    for (auto& combin : State)//çàïîëíÿþ ïåðâûå äâå ñòðîêè ó òàáëèöû
    {
        string newState = combin.first;
        string record = newState.substr(0, newState.find('/'));
        newState = newState.substr(newState.find('/') + 1);
        if (startState == record)
        {
            signal.insert(signal.begin() + 1, newState);
            headState.insert(headState.begin() + 1, combin.second);
        }
        else
        {
            signal.push_back(newState);
            headState.push_back(combin.second);
        }
    }
    result.push_back(signal);
    result.push_back(headState);
    for (int i = 2; i < Moore.size(); i++)
    {
        vector<string> fill = { Moore[i][0] };
        for (int l = 1; l < Moore[0].size(); l++)
        {
            string fillState = Moore[i][l];
            for (auto& combin : State)
            {
                string newState = combin.first;
                string record = newState.substr(0, newState.find('/'));
                if (fillState == record)
                {
                    fill.push_back(combin.second);
                    break;
                }
            }
        }
        result.push_back(fill);
    }
    return result;
}

void DeleteStatesMoore(vector<vector<string>>& result)
{
    map<string, string> newState;
    set<string> signal;
    for (int i = 1; i < result[0].size(); i++)
    {
        string nightState = "";
        for (int l = 1; l < result.size(); l++)
        {
            nightState += result[l][i];
        }
        if (signal.find(nightState) == signal.end())
        {
            signal.insert(nightState);
        }
        else
        {
            for (int k = 0; k < result.size(); k++)
            {
                result[k].erase(result[k].begin() + i);
            }
            i--;
        }
    }
}

void MinimizeMoore(vector<vector<string>>& TableMoore)
{
    map<string, string> State;
    string newState = "A";
    GetStateMoore(State, TableMoore, newState);
    vector<vector<string>> result = FillTheTableMoore(State, TableMoore);
    DeleteStatesMoore(result);
    vector<vector<string>> comparison = result;
    bool flag = true;
    while (flag)
    {
        State.clear();
        GetStateMoore(State, result, newState);
        result = FillTheTableMoore(State, result);
        DeleteStatesMoore(result);
        if (result[0].size() == comparison[0].size())
        {
            flag = false;
        }
        comparison = result;
    }
    TableMoore = result;
}

void RemoveStatesMoore(vector<vector<string>>& TableMoore) {
    queue<string> stateQueue;
    set<string> reachableStates;
    string currentState;
    unsigned int i = 1;
    reachableStates.insert(TableMoore[1][1]);
    for (int l = 2; l < TableMoore.size(); l++)
    {
        stateQueue.push(TableMoore[l][1]);
    }
    while (!stateQueue.empty())
    {
        currentState = stateQueue.front();
        stateQueue.pop();
        i = 1;
        for (i; i < TableMoore[1].size(); i++)
        {
            if (currentState == TableMoore[1][i])
            {
                break;
            }
        }
        for (int l = 1; l < TableMoore[1][i].size(); l++)
        {
            if (reachableStates.find(currentState) == reachableStates.end())
            {
                reachableStates.insert(currentState);
                for (int k = 2; k < TableMoore.size(); k++)
                {
                    if (currentState != TableMoore[k][i])
                    {
                        stateQueue.push(TableMoore[k][i]);
                    }
                }
            }
        }
    }
    for (int l = 1; l < 2; l++)
    {
        for (int k = 1; k < TableMoore[0].size(); k++)
        {
            currentState = TableMoore[l][k];
            if (reachableStates.find(currentState) == reachableStates.end())
            {
                for (int m = 0; m < TableMoore.size(); m++)
                {
                    TableMoore[m].erase(TableMoore[m].begin() + k);
                }
                k--;
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

    if (workParam == MOORE_TO_MEALY_PARAM)//Ìèëè
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
        RemoveStatesMealy(table);
        MinimizeMealy(table);
        PrintFile(table, outputFile);
    }
    else//Ìóð
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
        RemoveStatesMoore(table);
        MinimizeMoore(table);
        PrintFile(table, outputFile);
    }

    return 0;
}
