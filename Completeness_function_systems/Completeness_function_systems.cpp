#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

static vector<char> getBinaryFunction()
{
    string input;
    vector<char> binaryVector;

    while (true) {
        cout << "Введите двоичную строку: ";
        cin.seekg(cin.eof());
        getline(cin, input);

        input.erase(remove(input.begin(), input.end(), ' '), input.end());

        bool validInput = true;
        for (char ch : input) {
            if (ch != '0' && ch != '1') {
                validInput = false;
                break;
            }
        }

        if (validInput) {
            for (char ch : input) {
                binaryVector.push_back(ch);
            }

            int size = binaryVector.size();
            if ((size & (size - 1)) == 0) {
                break;
            }
            else {
                binaryVector.clear();
                cout << "Ошибка: количество элементов должно быть 2^n. Попробуйте снова." << endl;
            }
        }
        else {
            cout << "Ошибка: строка должна содержать только 0 и 1. Попробуйте снова." << endl;
        }
    }
    return binaryVector;
}

static bool T0Check(const vector<char>& function)
{
    return function[0] == '0';
}

static bool T1Check(const vector<char>& function) 
{
    return function[function.size() - 1] == '1';
}

static bool selfdualityCheck(const vector<char>& function) 
{
    if (function.size() == 1) return false;

    for (int i = 0; i < function.size() / 2; i++) {
        if (function[i] == function[function.size()-1-i]) {
            return false;
        }
    }
    return true;
}

static bool monotonicityCheck(const vector<char>& function)
{
    for (int i = 0; i < (1 << (function.size())); i++) {
        int previous_j = 0;
        for (int j = 0; j < function.size(); j++) {
            if ((i & (1 << j)) && (i & (1 << 0)) && (i & (1 << function.size() - 1))) {
                if ((previous_j & j) != previous_j) break;
                if (function[previous_j] > function[j]) return false;
                previous_j = j;
            }
        }
    }
    return true;
}

static bool linearСheck(vector<char> function_for_summ)
{
    vector<char> temp_func_for_summ;
    int iteration_count = function_for_summ.size();

    for (int i = 0; i < iteration_count; i++) {
        char constant = function_for_summ[0];

        for (int j = 1; j < function_for_summ.size(); j++) {
            constant ^= function_for_summ[j];
            temp_func_for_summ.push_back(constant);
            constant = function_for_summ[j];
        }

        if (function_for_summ[0] && i > 0 && (i & (i - 1)) != 0) return false;

        function_for_summ = temp_func_for_summ;
        temp_func_for_summ.clear();
    }
    return true;
}

static void printComplitnessTable(const vector<vector<bool>>& complitness_table, const vector<string>& line_headers = {})
{
    int cellWidth = 4;
    vector<string> column_headers = {"T0", "T1", "S", "M", "L"};
    cout << setw(cellWidth) << "";
    for (int i = 0; i < column_headers.size(); i++) cout << setw(cellWidth) << left << column_headers[i];
    cout << endl;

    for (int i = 0; i < complitness_table.size(); i++) {
        if (line_headers.size() == 0) {
            cout << setw(cellWidth) << left << "f" + to_string(i + 1);
        }
        else {
            cout << setw(cellWidth) << left << line_headers[i];
        }
        for (int j = 0; j < 5; j++) {
            complitness_table[i][j] ? cout << setw(cellWidth) << left << "+" : cout << setw(cellWidth) << left << "-";
        }
        cout << endl;
    }
}

static bool isComplitnessSystem(const vector<vector<bool>>& complitness_table)
{
    vector<bool> belong_vect(5, true);
    for (int i = 0; i < belong_vect.size(); i++) {
        for (int j = 0; j < complitness_table.size(); j++) {
            if (complitness_table[j][i] == false) {
                belong_vect[i] = false;
                break;
            }
        }
    }
    return count(belong_vect.begin(), belong_vect.end(), false) == belong_vect.size();
}

static vector<vector<vector<bool>>> getAllBasisSystem(const vector<vector<bool>>& complitness_table, 
                                                            vector<vector<string>>& line_headers)
{
    int min_complitness_system_size = complitness_table.size();
    vector<vector<vector<bool>>> allBasis;

    for (int i = 0; i < (1 << complitness_table.size()); i++) {
        vector<vector<bool>> potencial_basis;
        vector<string> line_header;
        for (int j = 0; j < complitness_table.size(); j++) {
            if (i & (1 << j)) {
                potencial_basis.push_back(complitness_table[j]);
                line_header.push_back("f"+to_string(j+1));
            }
        }
        if (isComplitnessSystem(potencial_basis)) {
            if (potencial_basis.size() < min_complitness_system_size) {
                allBasis.clear();
                line_headers.clear();
                allBasis.push_back(potencial_basis);
                line_headers.push_back(line_header);
                min_complitness_system_size = potencial_basis.size();
                
            }
            else if (potencial_basis.size() == min_complitness_system_size && 
                potencial_basis.size() != complitness_table.size()){
                allBasis.push_back(potencial_basis);
                line_headers.push_back(line_header);
            }
        }
    }

    return allBasis;
}

static void checkFunctionComplitness(const vector<vector<char>>& functions)
{
    vector<vector<bool>> complitness_table(functions.size(), vector<bool> (5, false));

    for (int i = 0; i < complitness_table.size(); i++) {
        complitness_table[i][0] = T0Check(functions[i]);
        complitness_table[i][1] = T1Check(functions[i]);
        complitness_table[i][2] = selfdualityCheck(functions[i]);
        complitness_table[i][3] = monotonicityCheck(functions[i]);
        complitness_table[i][4] = linearСheck(functions[i]);
    }

    printComplitnessTable(complitness_table);

    if (isComplitnessSystem(complitness_table)) {
        cout << "Система функций полная.\nБазис(ы): ";
        vector<vector<string>> line_headers;
        vector<vector<vector<bool>>> allBasis = getAllBasisSystem(complitness_table, line_headers);
        if (allBasis.size()) {
            for (int i = 0; i < allBasis.size(); i++) {
                cout << endl;
                printComplitnessTable(allBasis[i], line_headers[i]);
            }
        }
        else {
            cout << "Система уже является базисом.\n";
        }
    }
    else {
        cout << "Система функций не полная.\n";
    }
}

int main()
{
    setlocale(LC_ALL, "Rus");
    int count_func;
    vector<vector<char>> functions;
    
    cout << "Кол-во функций: "; cin >> count_func;

    for (int i = 0; i < count_func; i++) {
        functions.push_back(getBinaryFunction());
    }

    checkFunctionComplitness(functions);
}