#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <unordered_set>
#include <map>

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
    vector<Robot> robots;

    CELLTYPE getCellType(array<int, 2> & pos) const
    {
        return grid.at(pos.at(0)).at(pos.at(1));
    }

    int getRobotScore(Robot & robot)
    {
        int score = 0;

        // Loop until the robot is in the void or repeats a state
        while (true)
        {
            // If the robot is in the void, it's done
            auto cellType = getCellType(robot.pos);
            if (cellType == VOID)
            {
                return score;
            }

            // Turn the robot if necessary
            if (cellType == LEFTARROW) { robot.direction = LEFT; }
            else if (cellType == RIGHTARROW) { robot.direction = RIGHT; }
            else if (cellType == UPARROW) { robot.direction = UP; }
            else if (cellType == DOWNARROW) { robot.direction = DOWN; }

            // If the robot has returned to a previous state, it's done
            int robotStateID = robot.getStateID();
            if (robot.visitedStates.count(robotStateID))
            {
                return score;
            }

            // If the robot survives, get a point
            ++score;

            // Log that the robot has visited this state
            robot.visitedStates.insert(robotStateID);

            // Advance the robot and repeat
            if (robot.direction == LEFT) 
            {
                int& coord = robot.pos.at(1);
                coord = coord != 0 ? coord - 1 : 18;
            }
            else if (robot.direction == RIGHT)
            {
                int& coord = robot.pos.at(1);
                coord = coord != 18 ? coord + 1 : 0;
            }
            else if (robot.direction == UP)
            {
                int& coord = robot.pos.at(0);
                coord = coord != 0 ? coord - 1 : 9;
            }
            else if (robot.direction == DOWN)
            {
                int& coord = robot.pos.at(0);
                coord = coord != 9 ? coord + 1 : 0;
            }
        }
    }

    int getSimScore()
    {
        // Simulate the game and predict the final score
        int score = 0;

        // Cycle through the robots
        for (auto & robot : robots)
        {
            score += getRobotScore(robot);
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