#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <unordered_set>
#include <map>
#include <list>

using namespace std;

enum CELLTYPE
{
    PLATFORM,
    LEFTARROW,
    RIGHTARROW,
    UPARROW,
    DOWNARROW,
    VOID
};

enum DIRECTION
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

map<char, CELLTYPE> cellMap =
{
    { '#', VOID },
{ '.', PLATFORM },
{ 'L', LEFTARROW },
{ 'R', RIGHTARROW },
{ 'U', UPARROW },
{ 'D', DOWNARROW }
};

map<char, DIRECTION> dirMap =
{
    { 'L', LEFT },
{ 'R', RIGHT },
{ 'U', UP },
{ 'D', DOWN }
};


struct Robot
{
    array<int, 2> pos;
    DIRECTION direction;
    unordered_set<int> visitedStates;

    int getStateID()
    {
        // Return an int that is unique for the input position/direction state of the robot
        // There are 190 grid positions, so multiply the direction by 200
        return direction * 200 + pos.at(0) * 19 + pos.at(1);
    }
};

struct Board
{
    array<array<CELLTYPE, 19>, 10> grid;
    list<Robot> robots;

    CELLTYPE getCellType(array<int, 2> & pos) const
    {
        return grid.at(pos.at(0)).at(pos.at(1));
    }

    int getSimScore()
    {
        // Simulate the game and predict the final score
        int score = 0;

        // Cycle through the robots
        auto robotIter = robots.begin();
        while (robotIter != robots.end())
        {
            // Delete any robots in the void
            auto cellType = getCellType(robotIter->pos);
            if (cellType == VOID)
            {
                robotIter = robots.erase(robotIter);
                continue;
            }

            // Turn the robot if necessary
            if (cellType == LEFTARROW) { robotIter->direction = LEFT; }
            else if (cellType == RIGHTARROW) { robotIter->direction = RIGHT; }
            else if (cellType == UPARROW) { robotIter->direction = UP; }
            else if (cellType == DOWNARROW) { robotIter->direction = DOWN; }

            // Delete any robots that have returned to an identical state
            int robotStateID = robotIter->getStateID();
            if (robotIter->visitedStates.count(robotStateID))
            {
                robotIter = robots.erase(robotIter);
                continue;
            }

            // If the robot survives, get a point
            ++score;

            // Log that the robot has visited this state
            robotIter->visitedStates.insert(robotStateID);

            // TODO: Advance the robot and repeat
            // I can do each robot to completion before moving on to the next. This means I don't need to delete robots on death
            // use vector, and a for each loop


            ++robotIter;
        }

        return score;
    }
};

int main()
{
    Board board;

    for (int i = 0; i < 10; i++)
    {
        string line;
        getline(cin, line);

        for (int j = 0; j < 19; j++)
        {
            char& ch = line.at(j);
            board.grid.at(i).at(j) = cellMap.at(ch);
        }
    }

    int robotCount;
    cin >> robotCount; cin.ignore();
    Robot tempRobot;
    for (int i = 0; i < robotCount; i++) {
        int x;
        int y;
        string direction;
        cin >> x >> y >> direction; cin.ignore();

        tempRobot.pos = { {y, x} };
        tempRobot.direction = dirMap.at(direction[0]);
        board.robots.push_back(tempRobot);
    }

    cerr << board.getSimScore() << endl;

    cout << endl;
}