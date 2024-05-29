#include <conio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include<iostream>
#include <ctime>
#include <queue>
#include "Header.h"

int n, m, x = 0, y = 0, tick = 300;
COORD position, newPosition;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);// получение дескриптора устройства стандартного вывода

enum TypeCell {
    dead,
    alive,
    predator,
    block,
    head
};
struct Pred_cell;

struct Cell {
    TypeCell typeCell = TypeCell::dead;
    int lifetime = 0;
    friend ostream& operator<<(ostream& out, const Cell& cell) {
        if (cell.typeCell == TypeCell::dead)
            out << '.';
        else if (cell.typeCell == TypeCell::alive)
            out << '#';
        else if (cell.typeCell == TypeCell::predator)
            out << '$';
        else if (cell.typeCell == TypeCell::block)
            out << '|';
        else if (cell.typeCell == TypeCell::head)
            out << 'O';
        return out;
    }



    Cell& operator=(const Pred_cell& pred) {
        this->typeCell = TypeCell::predator;
        this->lifetime = 0;
        return *this;
    }
};

struct Pred_cell : public Cell
{
    Pred_cell(const Cell& cell)
    {
        this->typeCell = predator;
        this->lifetime = cell.lifetime;
    };

    Pred_cell()
    {
        this->lifetime = 0;
        this->typeCell = predator;
    }




    friend ostream& operator<<(ostream& out, const Pred_cell& pred_cell) {
        out << '$';
        return out;
    }
    Pred_cell& operator=(const Cell& cell) {
        this->typeCell = TypeCell::dead;
        this->lifetime = 0;
        return *this;
    }

};



class Field
{
private:
    int n;
    int m;
public:
    Array<Array<Cell>> cells;
    Field() {};
    Field(int n, int m, float aliveProportion = 0.3f) : n(n), m(m) {
        srand(0);
        cells = Array<Array<Cell>>(n);
        for (int i = 0; i < n; i++) {
            cells[i] = Array<Cell>(m);
            for (int j = 0; j < m; j++) {
                Cell cell;
                float prop = (rand() % 10000) / 10000.f;
                if (prop <= aliveProportion)
                    cell.typeCell = TypeCell::alive;
                else
                    cell.typeCell = TypeCell::dead;
                cells[i][j] = cell;
            }

            for (int j = 0; j < m; j++)
            {
                if (rand() % 10 >= 8)
                {
                    Pred_cell pred;
                    cells[i][j] = pred;
                }
            }

        }
    }

    int sizeh() const
    {
        return this->n;
    }

    int sizew() const
    {
        return this->m;
    }

    friend ostream& operator<<(ostream& out, const Field& field) {
        for (int i = 0; i < field.n; i++) {
            for (int j = 0; j < field.m; j++) {
                if (field.cells[i][j].typeCell == TypeCell::dead)
                    out << ".";
                else if (field.cells[i][j].typeCell == TypeCell::alive)
                    out << "#";
                else if (field.cells[i][j].typeCell == TypeCell::predator)
                    out << '$';
                else if (field.cells[i][j].typeCell == TypeCell::block)
                    out << '|';
                else if (field.cells[i][j].typeCell == TypeCell::head)
                {
                    out << 'O';
                }
            }
            out << endl;
        }
        return out;
    }

    Array<Cell>& operator[](int i)
    {
        return  cells[i];
    }

    int getNum(int posX, int posY, TypeCell type = alive, int radius = 1) const {
        int count = 0;
        int i = posY - radius;
        int j = posX - radius;
        int I = posY + radius;
        int J = posX + radius;
        while (i <= I)
        {
            if (i >= 0 && i <= n - 1)
            {
                while (j <= J)
                {
                    if (j >= 0 && j <= m - 1)
                    {
                        if ((posY == i && posX == j))
                        {
                            j++;
                            continue;
                        }

                        if (cells[i][j].typeCell == dead)
                        {
                            j++;
                        }
                        else if (cells[i][j].typeCell == alive)
                        {
                            j++;
                            count++;
                        }
                        else
                            j++;
                    }
                    else
                    {
                        j++;
                    }

                }
            }
            else
            {

            }
            i++;
            j = posX - radius;
        }
        return count;
    }

    bool check_place(int x, int y)
    {
        if (x < m && x >= 0 && y < n && y >= 0)
            return true;
        return false;
    }

    pair<int, int> move_for_pred(int x0, int y0)
    {
        pair<int, int> move_ar[9] = { {-1, 0} , {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1} };
        int t = rand() % 8;
        int time = t;
        int flag = 0;
        pair<int, int> next_coord = { x0 + move_ar[t].first, y0 + move_ar[t].second };
        while (!check_place(next_coord.first, next_coord.second) && flag < 9) {

            time = (time + 1) % 8;
            flag++;
            next_coord = { x0 + move_ar[time].first, y0 + move_ar[time].second };
        }
        if (flag == 9)
            return { x0,y0 };
        if ((this->cells[next_coord.second][next_coord.first].typeCell == block) || (this->cells[next_coord.second][next_coord.first].typeCell == predator))
            return move_for_pred(x0, y0);
        
        return next_coord;
    }


    void division_pred(int y, int x, Field& baza)
    {
        if (baza[y][x].typeCell == predator)
        {

        }
        else if (baza[y][x].typeCell == dead)
        {
            baza[y][x].typeCell = predator;
            baza[y][x].lifetime = 0;
        }
        else
        {
            baza[y][x].typeCell = predator;
            baza[y][x].lifetime = 0;
            pair<int, int> move = move_for_pred(x, y);
            if (rand() % 10 >= 5)
            {
                division_pred(move.second, move.first, baza);
            }

        }
    }


    void move_pred(Field& baza, Pred_cell test, int x0, int y0, int x, int y, double division_chance = 0.5)
    {
        pair<int, int> move;
        move.first = x;
        move.second = y;
        Cell cell;
        if (baza[y][x].typeCell == alive)
        {
            baza[y][x] = test;
            baza[y0][x0] = cell;
            if (rand() % 10 >= division_chance * 10)
            {
                pair<int, int> div = move_for_pred(x, y);
                int next_pred_x = div.first;
                int next_pred_y = div.second;
                baza.division_pred(next_pred_y, next_pred_x, baza);

            }

        }
        else if (baza[y][x].typeCell == dead)
        {
            baza[y][x] = test;
            baza[y][x].lifetime = test.lifetime + 1;
            int t = baza[y][x].lifetime;
            baza[y0][x0] = cell;
        }
        else
        {
            if (move.first == x0 && move.second == y0)
            {
                baza[y][x].lifetime = test.lifetime + 1;

            }
            else
            {
                baza[y0][x0] = cell;
            }
        }


    }
};


struct iGame {
    int n = 0;
    int m = 0;



    int seed = 0; // случайная величина для генератора
    double aliveProportion = 0.0;  // вероятность того, что клетка живая
    int dimension = 2; // размерность

    int radius = 1; // радиус проверки, граница включена
    int loneliness = 1; // с этого числа и меньше клетки умирают от одиночества
    int birth_start = 3; // с этого числа и до birth_end появляется живая клетка
    int birth_end = 3;
    int overpopulation = 4; // с этого числа и дальше клетки погибают от перенаселения
    int rand_spawn = 1;
    int enxtintion = 4;

    double devision_chance = 0.5;

    double chance_of_block = 0.4;
    virtual void runGame(int numIt) = 0;
    virtual bool save(const string& filename) const = 0;
    virtual bool load(const string& filename) = 0;
};

void updateAllConsole(Field maingame) {
    cout << maingame;
    SetConsoleCursorPosition(hConsole, position);
}

struct Game : public iGame {
protected:
    virtual void runstep(Field a)
    {

        Field field = a;

        for (int i = 0; i < a.sizeh(); i++)
        {
            for (int j = 0; j < a.sizew(); j++)
            {
                if (a[i][j].typeCell == predator)
                {
                    if (a[i][j].lifetime >= iGame::enxtintion)
                    {
                        field[i][j].lifetime = 0;
                        field[i][j].typeCell = dead;
                    }
                    else
                    {
                        Pred_cell unit;
                        unit.lifetime = a[i][j].lifetime;
                        int h = unit.lifetime;
                        pair<int, int> next_move = field.move_for_pred(j, i);
                        maingame.move_pred(field, unit, j, i, next_move.first, next_move.second);
                    }
                }
            }
        }
        updateAllConsole(field);


        a = field;
        updateAllConsole(a);


        for (int i = 0; i < a.sizeh(); i++)
        {
            for (int j = 0; j < a.sizew(); j++)
            {
                if (a[i][j].typeCell == dead)
                {
                    if (a.getNum(j, i) >= iGame::birth_start && a.getNum(j, i) <= iGame::birth_end)
                    {
                        field[i][j].typeCell = alive;
                    }
                    else
                        field[i][j].typeCell = dead;
                }
                else if (a[i][j].typeCell == alive)
                {
                    if (a.getNum(j, i) > iGame::loneliness && a.getNum(j, i) < iGame::overpopulation)
                    {
                        field[i][j].typeCell = alive;
                    }
                    else
                        field[i][j].typeCell = dead;
                }
            }
        }
        a = field;
        maingame = field;


    }
    Field maingame;



    bool save(const string& saves) const override {
        ofstream out;
        out.open("saves.txt");
        if (out.is_open())
        {
            out << maingame.sizeh() << ' ' << maingame.sizew() << ' ' << '#' << ' ' << '.' << ' ' << '$' << endl;
            for (int i = 0; i < maingame.sizeh(); i++)
            {
                for (int j = 0; j < maingame.sizew(); j++)
                {
                    out << maingame.cells[i][j];
                }
                out << endl;
            }
            out.close();
            return true;
        }
        return false;


    }
    bool load(const string& saves) override {
        ifstream in("saves.txt");
        if (in.is_open())
        {
            string first;
            getline(in, first);
            int rows = 0;
            int col = 0;
            int i = 0;
            string strnum;
            int number = 0;

            while (first[i] != ' ')
            {

                strnum.push_back(first[i]);
                i++;
            }
            number = stoi(strnum);

            rows = number;
            number = 0;
            i++;
            strnum = "";

            while (first[i] != ' ')
            {

                strnum.push_back(first[i]);
                i++;
            }

            number = stoi(strnum);
            col = number;
            i++;
            char alive = first[i];
            i++;
            char dead = first[i];

            Field temp(rows, col);

            for (int i = 0; i < rows; i++)
            {
                getline(in, first);
                for (int j = 0; j < col; j++)
                {
                    if (first[j] == alive)
                    {
                        temp[i][j].typeCell = TypeCell::alive;
                    }
                    else
                        temp[i][j].typeCell = TypeCell::dead;
                }
            }
            maingame = temp;

            updateAllConsole(maingame);
        }
        return false;
    }
public:
    Game() {};
    Game(int n, int m) {
        maingame = Field(n, m);

        updateAllConsole(maingame);
        Sleep(1500);
    }

    void runGame(int numIt) override {
        int i = 0;
        int button;
        bool flag = 0;
        bool flag_for_space = 0;
        bool flag_main = 0;
        while (i < numIt)
        {
            if (_kbhit())
            {

                button = _getch();
                if (button == 32 && flag_for_space == 0)
                {

                    while (flag_for_space == 0)
                    {
                        button = 0;
                        while (button != 32)
                        {
                            if (_kbhit())
                                button = _getch();


                            if (button == 'd') x = (x + 1 + maingame.sizew()) % maingame.sizew();
                            else if (button == 'a') x = (x - 1 + maingame.sizew()) % maingame.sizew();
                            else if (button == 'w') y = (y - 1 + maingame.sizeh()) % maingame.sizeh();
                            else if (button == 's') y = (y + 1 + maingame.sizeh()) % maingame.sizeh();
                            else if (button == 'c')
                            {
                                if (maingame[y][x].typeCell == dead)
                                {
                                    maingame[y][x].typeCell = alive;
                                    putchar('#');
                                }
                                else if (maingame[y][x].typeCell == alive)
                                {
                                    maingame[y][x] = (Pred_cell)maingame[y][x];
                                    maingame[y][x].lifetime = 0;
                                    putchar('$');
                                }
                                else
                                {
                                    maingame[y][x].typeCell = dead;
                                    putchar('.');
                                }

                            }
                            position.X = x;
                            position.Y = y;


                            SetConsoleCursorPosition(hConsole, position);





                            if (button == 27)
                            {

                                position.X = 0;
                                position.Y = maingame.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);
                                cout << "wasted";
                                Sleep(1000);
                                flag_main = 1;
                                flag_for_space = 1;
                                break;
                            }
                            else if ((char)button == 'p')
                            {
                                save("saves.txt");

                                position.X = 0;
                                position.Y = maingame.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);
                                cout << "saved";
                                Sleep(1000);
                                position.X = 0;
                                position.Y = maingame.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);

                                cout << "      ";

                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                button = 0;
                            }
                            else if ((char)button == 'l')
                            {
                                load("saves.txt");
                                position.X = 0;
                                position.Y = maingame.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);
                                cout << "loaded";
                                Sleep(1000);
                                position.X = 0;
                                position.Y = maingame.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);

                                cout << "      ";

                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                button = 0;
                            }
                            if (button == 32)
                            {
                                flag_for_space = 1;
                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                continue;
                            }
                            button = 0;
                        }
                    }
                }


            }
            else
            {
                runstep(maingame);
                i++;
                updateAllConsole(maingame);
                Sleep(1500);
                flag_for_space = 0;
            }
            if (flag_main == 1)
                break;
        }

    }
    friend Field;


};



pair<Array<Array<int>>, vector<vector<int>>> lists(Field field)
{
    Array < Array<int>> temp;
    temp = Array<Array<int>>(field.sizeh());

    int count = 0;
    int x = 0, y = 0;
    for (int i = 0; i < field.sizeh(); i++)
    {
        temp[i] = Array<int>(field.sizew());
        for (int j = 0; j < field.sizew(); j++)
        {
            if (field[i][j].typeCell != block)
            {
                temp[i][j] = count;
                count++;

            }
            else
            {
                temp[i][j] = -1;

            }

        }
    }

    vector<vector<int>> ans(count);
    int h = 0;

    for (int i = 0; i < field.sizeh(); i++)
    {
        for (int j = 0; j < field.sizew(); j++)
        {
            if (temp[i][j] != -1)
            {
                for (int k = -1; k < 2; k++)
                {

                    for (int l = -1; l < 2; l++)
                    {
                        if (!(k == 0 && l == 0))
                        {
                            if (field.check_place(j + l, i + k))
                            {
                                if (field[i + k][j + l].typeCell != block)
                                {
                                    ans[temp[i][j]].push_back(temp[i + k][j + l]);

                                }
                            }
                        }

                    }
                }
            }

        }
    }
    pair<Array<Array<int>>, vector<vector<int>>> t(temp, ans);
    return t;
}



pair<int, int> num_to_coord(Array<Array<int>> general, int num, Field field)
{
    for (int i = 0; i < field.sizeh(); i++)
    {
        for (int j = 0; j < field.sizew(); j++)
        {
            if (general[i][j] == num) return make_pair(i, j);

        }
    }
}


pair<int, int> bfs(vector<vector<int>>& graph, int start, Field field, Array<Array<int>> general)
{
    vector<bool> visited(graph.size());
    vector<vector<int>> dist(graph.size());
    queue<int> q;

    int x0 = num_to_coord(general, start, field).second;
    int y0 = num_to_coord(general, start, field).first;

    visited[start] = true;
    dist[start].push_back(start);
    q.push(start);

    while (!q.empty())
    {
        int cur = q.front();
        q.pop();
        for (int t : graph[cur])
        {
            int y = num_to_coord(general, t, field).first;
            int x = num_to_coord(general, t, field).second;
            if (!visited[t])
            {
                visited[t] = true;
                for (int y : dist[cur])
                    dist[t].push_back(y);
                dist[t].push_back(t);
                q.push(t);
            }

            if (field[y][x].typeCell == alive)
            {

                int r = dist[t][1];

                int y1 = num_to_coord(general, r, field).first;
                int x1 = num_to_coord(general, r, field).second;

                pair<int, int> ans;
                ans.first = y1;
                ans.second = x1;
                return ans;
            }
        }
    }
    return make_pair(y0, x0);
}

struct Smart_hunt : protected Game
{
private:
    Array<Array<int>> graph;
    vector<vector<int>> smej;

    Field tema;


    void runstep(Field help)
    {
        for (int i = 0; i < tema.sizeh(); i++)
        {
            for (int j = 0; j < tema.sizew(); j++)
            {
                if (tema[i][j].typeCell == predator)
                {
                    if (tema[i][j].lifetime >= iGame::enxtintion)
                    {
                        help[i][j].lifetime = 0;
                        help[i][j].typeCell = dead;
                    }
                    else
                    {
                        pair<int, int> move = bfs(smej, graph[i][j], help, graph);
                        Pred_cell unit;
                        unit.lifetime = tema[i][j].lifetime;
                        int h = unit.lifetime;
                        help.move_pred(help, unit, j, i, move.second, move.first);
                    }
                }
            }
        }
        tema = help;
        updateAllConsole(tema);

        for (int i = 0; i < tema.sizeh(); i++)
        {
            for (int j = 0; j < tema.sizew(); j++)
            {
                if (help[i][j].typeCell == dead)
                {
                    if (help.getNum(j, i) >= iGame::birth_start && help.getNum(j, i) <= iGame::birth_end)
                    {
                        tema[i][j].typeCell = alive;
                    }
                    else if (rand() % 100 <= iGame::rand_spawn)
                        tema[i][j].typeCell = alive;
                    else
                        tema[i][j].typeCell = dead;
                }
                else if (tema[i][j].typeCell == alive)
                {
                    if (help.getNum(j, i) > iGame::loneliness && help.getNum(j, i) < iGame::overpopulation)
                    {
                        tema[i][j].typeCell = alive;
                    }
                    else
                        tema[i][j].typeCell = dead;
                }
            }
        }
    }


    // save/ load
    bool save(const string& saves) const override {
        ofstream out;
        out.open("saves.txt");
        if (out.is_open())
        {
            out << tema.sizeh() << ' ' << tema.sizew() << ' ' << '.' << ' ' << '#' << ' ' << '$' << ' ' << '|'<<' '<< 'O' << endl;
            for (int i = 0; i < tema.sizeh(); i++)
            {
                for (int j = 0; j < tema.sizew(); j++)
                {
                    out << tema.cells[i][j];
                }
                out << endl;
            }
            out.close();
            return true;
        }
        return false;
    }

    bool load(const string& saves) override {
        ifstream in("saves.txt");
        if (in.is_open())
        {
            string first;
            getline(in, first);
            int rows = 0;
            int col = 0;
            int i = 0;
            string strnum;
            int number = 0;

            while (first[i] != ' ')
            {

                strnum.push_back(first[i]);
                i++;
            }
            number = stoi(strnum);

            rows = number;
            number = 0;
            i++;
            strnum = "";

            while (first[i] != ' ')
            {

                strnum.push_back(first[i]);
                i++;
            }

            number = stoi(strnum);
            col = number;
            i++;
            char dead = first[i];
            i += 2;
            char alive = first[i];
            i += 2;
            char pred = first[i];
            i += 2;
            char block = first[i];
            i += 2;
            char head = first[i];

            Field temp(rows, col);

            for (int i = 0; i < rows; i++)
            {
                getline(in, first);
                for (int j = 0; j < col; j++)
                {
                    if (first[j] == alive)
                    {
                        temp[i][j].typeCell = TypeCell::alive;
                    }
                    else if (first[j] == dead)
                        temp[i][j].typeCell = TypeCell::dead;
                    else if (first[j] == pred)
                        temp[i][j].typeCell = TypeCell::predator;
                    else if (first[j] == block)
                        temp[i][j].typeCell = TypeCell::block;
                    else if (first[j] == head)
                        temp[i][j].typeCell = TypeCell::head;
                }
            }
            tema = temp;
        }
        return false;
    }

public:
    Smart_hunt(int n, int m)
    {
        tema = Field(n, m);

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                if (rand() % 10 > chance_of_block * 10)
                {
                    tema[i][j].typeCell = block;
                }
            }
        }

        updateAllConsole(tema);
        Sleep(1500);
        graph = lists(tema).first;
        smej = lists(tema).second;
    };
    void runGame(int numIt) override {
        int i = 0;
        int button;
        bool flag = 0;
        bool flag_for_space = 0;
        bool flag_main = 0;
        while (i < numIt)
        {
            if (_kbhit())
            {

                button = _getch();
                if (button == 32 && flag_for_space == 0)
                {

                    while (flag_for_space == 0)
                    {
                        button = 0;
                        while (button != 32)
                        {
                            if (_kbhit())
                                button = _getch();


                            if (button == 'd') x = (x + 1 + tema.sizew()) % tema.sizew();
                            else if (button == 'a') x = (x - 1 + tema.sizew()) % tema.sizew();
                            else if (button == 'w') y = (y - 1 + tema.sizeh()) % tema.sizeh();
                            else if (button == 's') y = (y + 1 + tema.sizeh()) % tema.sizeh();
                            else if (button == 'c')
                            {
                                if (tema[y][x].typeCell == dead)
                                {
                                    tema[y][x].typeCell = alive;
                                    putchar('#');
                                }
                                else if (tema[y][x].typeCell == alive)
                                {
                                    tema[y][x] = (Pred_cell)tema[y][x];
                                    tema[y][x].lifetime = 0;
                                    putchar('$');
                                }
                                else if (tema[y][x].typeCell == predator)
                                {
                                    tema[y][x].typeCell = block;
                                    putchar('|');
                                }
                                else if (tema[y][x].typeCell == block)
                                {
                                    tema[y][x].typeCell = dead;
                                    putchar('.');
                                }

                            }
                            position.X = x;
                            position.Y = y;
                            SetConsoleCursorPosition(hConsole, position);
                            if (button == 27)
                            {

                                position.X = 0;
                                position.Y = tema.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);
                                cout << "wasted";
                                Sleep(1000);
                                flag_main = 1;
                                flag_for_space = 1;
                                break;
                            }
                            else if ((char)button == 'p')
                            {
                                save("saves.txt");

                                position.X = 0;
                                position.Y = tema.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);
                                cout << "saved";
                                Sleep(1000);
                                position.X = 0;
                                position.Y = tema.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);

                                cout << "      ";

                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                button = 0;
                            }
                            else if ((char)button == 'l')
                            {
                                load("saves.txt");


                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                updateAllConsole(tema);
                                button = 0;
                            }
                            if (button == 32)
                            {
                                flag_for_space = 1;
                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                continue;
                            }
                            button = 0;
                        }
                    }
                }


            }
            else
            {
                runstep(tema);
                i++;
                updateAllConsole(tema);
                Sleep(1500);
                flag_for_space = 0;
            }
            if (flag_main == 1)
                break;
        }

    }
};

struct Snake : protected Game
{

    Field tema;
    vector<pair<int, int>> co;

    bool save(const string& saves) const override {
        ofstream out;
        out.open("saves.txt");
        if (out.is_open())
        {
            out << tema.sizeh() << ' ' << tema.sizew() << ' ' << '.' << ' ' << '#' << ' ' << '$' << ' ' << '|' << ' ' << 'O' << endl;
            for (int i = 0; i < tema.sizeh(); i++)
            {
                for (int j = 0; j < tema.sizew(); j++)
                {
                    out << tema.cells[i][j];
                }
                out << endl;
            }
            out.close();
            return true;
        }
        return false;
    }

    bool load(const string& saves) override {
        ifstream in("saves.txt");
        if (in.is_open())
        {
            string first;
            getline(in, first);
            int rows = 0;
            int col = 0;
            int i = 0;
            string strnum;
            int number = 0;

            while (first[i] != ' ')
            {

                strnum.push_back(first[i]);
                i++;
            }
            number = stoi(strnum);

            rows = number;
            number = 0;
            i++;
            strnum = "";

            while (first[i] != ' ')
            {

                strnum.push_back(first[i]);
                i++;
            }

            number = stoi(strnum);
            col = number;
            i++;
            char dead = first[i];
            i += 2;
            char alive = first[i];
            i += 2;
            char pred = first[i];
            i += 2;
            char block = first[i];
            i += 2;
            char head = first[i];

            Field temp(rows, col);

            for (int i = 0; i < rows; i++)
            {
                getline(in, first);
                for (int j = 0; j < col; j++)
                {
                    if (first[j] == alive)
                    {
                        temp[i][j].typeCell = TypeCell::alive;
                    }
                    else if (first[j] == dead)
                        temp[i][j].typeCell = TypeCell::dead;
                    else if (first[j] == pred)
                        temp[i][j].typeCell = TypeCell::predator;
                    else if (first[j] == block)
                        temp[i][j].typeCell = TypeCell::block;
                    else if (first[j] == head)
                        temp[i][j].typeCell = TypeCell::head;
                }
            }
            tema = temp;
        }
        return false;
    }
    Snake(int n, int m)
    {
        bool flag = false;
        tema = Field(n, m);
        updateAllConsole(tema);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                if (tema[i][j].typeCell == predator)
                {
                    if (!flag && tema[i][j].typeCell == predator)
                    {
                            tema[i][j].typeCell = head;
                            co.push_back(make_pair(i, j));
                            flag++;
                    }
                    else
                    {
                        if (rand() % 10 > 6)
                        {
                            tema[i][j].typeCell = alive;
                        }
                        else
                            tema[i][j].typeCell = dead;
                    }
                }
            }
        }

        updateAllConsole(tema);
        Sleep(1500);

    };
    
    void erase_lifetime(Field& help)
    {
        for (int i = 0; i < help.sizeh(); i++)
        {
            for (int j = 0; j < help.sizew(); j++)
            {
                if (help[i][j].typeCell != head)
                    help[i][j].lifetime = 0;
            }
        }
    }


    void move_snake(Field& help,int y,int x)
    {
        pair<int, int>move = help.move_for_pred(x, y);

        bool flag = false;
        co.insert(co.begin(), make_pair(move.second, move.first));

        if (help[move.second][move.first].typeCell == alive)
        {
            help[move.second][move.first].typeCell = head;
            help[move.second][move.first].lifetime = 0;
            flag++;
            
        }
        else
        {
            help[move.second][move.first].typeCell = head;
            help[move.second][move.first].lifetime = help[y][x].lifetime;
            help[move.second][move.first].lifetime++;
        }
        if (co.size() <= 2)
        {
            if (!flag)
            {
                help[y][x].typeCell = dead;
                co.pop_back();
            }
            else
                help[y][x].typeCell = predator;
        }
        else
        {
            help[y][x].typeCell = predator;
            if (!flag)
            {
                pair<int, int> temp = co.back();
                help[temp.first][temp.second].typeCell = dead;
                co.pop_back();
            }
        }

        
        erase_lifetime(help);

    }
    void runstep(Field help)
    {
        bool flag = false;
        for (int i = 0; i < tema.sizeh(); i++)
        {
            for (int j = 0; j < tema.sizew(); j++)
            {
                if (tema[i][j].typeCell==head)
                {
                    if (tema[i][j].lifetime == iGame::enxtintion)
                    {
                        pair<int, int> temp = co.back();
                        co.pop_back();
                        help[temp.first][temp.second].typeCell = dead;
                        help[i][j].lifetime = 0;
                    }
                    if (co.size() != 0)
                    {
                        move_snake(help, i, j);
                        break;
                    }
                }
            }
        }

        tema = help;
        updateAllConsole(tema);

        for (int i = 0; i < tema.sizeh(); i++)
        {
            for (int j = 0; j < tema.sizew(); j++)
            {
                if (help[i][j].typeCell == dead)
                {
                    if (help.getNum(j, i) >= iGame::birth_start && help.getNum(j, i) <= iGame::birth_end)
                    {
                        tema[i][j].typeCell = alive;
                    }
                    else if (rand() % 100 <= iGame::rand_spawn)
                        tema[i][j].typeCell = alive;
                    else
                        tema[i][j].typeCell = dead;
                }
                else if (tema[i][j].typeCell == alive)
                {
                    if (help.getNum(j, i) > iGame::loneliness && help.getNum(j, i) < iGame::overpopulation)
                    {
                        tema[i][j].typeCell = alive;
                    }
                    else
                        tema[i][j].typeCell = dead;
                }
            }

        }
    }

    void runGame(int numIt) override {
        int i = 0;
        int button;
        bool flag = 0;
        bool flag_for_space = 0;
        bool flag_main = 0;
        while (i < numIt)
        {
            if (_kbhit())
            {

                button = _getch();
                if (button == 32 && flag_for_space == 0)
                {

                    while (flag_for_space == 0)
                    {
                        button = 0;
                        while (button != 32)
                        {
                            if (_kbhit())
                                button = _getch();


                            if (button == 'd') x = (x + 1 + tema.sizew()) % tema.sizew();
                            else if (button == 'a') x = (x - 1 + tema.sizew()) % tema.sizew();
                            else if (button == 'w') y = (y - 1 + tema.sizeh()) % tema.sizeh();
                            else if (button == 's') y = (y + 1 + tema.sizeh()) % tema.sizeh();
                            else if (button == 'c')
                            {
                                if (tema[y][x].typeCell == dead)
                                {
                                    tema[y][x].typeCell = alive;
                                    putchar('#');
                                }
                                else if (tema[y][x].typeCell == alive)
                                {
                                    tema[y][x] = (Pred_cell)tema[y][x];
                                    tema[y][x].lifetime = 0;
                                    putchar('$');
                                }
                                else if (tema[y][x].typeCell == predator)
                                {
                                    tema[y][x].typeCell = block;
                                    putchar('|');
                                }
                                else if (tema[y][x].typeCell == block)
                                {
                                    tema[y][x].typeCell = head;
                                    co.push_back(make_pair(y, x));
                                    putchar('O');
                                }
                                else if (tema[y][x].typeCell == head)
                                {
                                    tema[y][x].typeCell = dead;
                                    putchar('.');
                                }

                            }
                            position.X = x;
                            position.Y = y;
                            SetConsoleCursorPosition(hConsole, position);
                            if (button == 27)
                            {

                                position.X = 0;
                                position.Y = tema.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);
                                cout << "wasted";
                                Sleep(1000);
                                flag_main = 1;
                                flag_for_space = 1;
                                break;
                            }
                            else if ((char)button == 'p')
                            {
                                save("saves.txt");

                                position.X = 0;
                                position.Y = tema.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);
                                cout << "saved";
                                Sleep(1000);
                                position.X = 0;
                                position.Y = tema.sizeh() + 1;
                                SetConsoleCursorPosition(hConsole, position);

                                cout << "      ";

                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                button = 0;
                            }
                            else if ((char)button == 'l')
                            {
                                load("saves.txt");


                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                updateAllConsole(tema);
                                button = 0;
                            }
                            if (button == 32)
                            {
                                flag_for_space = 1;
                                position.X = 0;
                                position.Y = 0;
                                SetConsoleCursorPosition(hConsole, position);
                                continue;
                            }
                            button = 0;
                        }
                    }
                }


            }
            else
            {
                runstep(tema);
                i++;
                updateAllConsole(tema);
                Sleep(750);
                flag_for_space = 0;
            }
            if (flag_main == 1)
                break;
        }

    }
};

int main() {
    setlocale(0, "");
    int n = 5;
    int m = 5;

    srand(time(NULL));
    Smart_hunt test(20, 20);
    test.runGame(100);

}