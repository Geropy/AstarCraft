#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <unordered_set>
#include <map>
#include <ctime>

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

struct SimResult
{
    int score;
    array<int, 2> lastPos;

    SimResult()
        : score(0)
        , lastPos({ {0,0} })
    {}
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
    vector<Robot> robots, initRobots;

    void putCellType(array<int, 2> & pos, CELLTYPE type)
    {
        grid.at(pos.at(0)).at(pos.at(1)) = type;
    }

    CELLTYPE getCellType(array<int, 2> & pos) const
    {
        return grid.at(pos.at(0)).at(pos.at(1));
    }

    SimResult getRobotResult(Robot & robot)
    {
        SimResult result;

        // Loop until the robot is in the void or repeats a state
        while (true)
        {
            // If the robot is in the void, it's done
            auto cellType = getCellType(robot.pos);
            if (cellType == VOID)
            {
                return result;
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
                return result;
            }

            // If the robot survives, get a point
            ++result.score;

            // Log that the robot has visited this state, and note this as the last safe position so far (if an empty platform)
            robot.visitedStates.insert(robotStateID);
            if (cellType == PLATFORM) { result.lastPos = robot.pos; }

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

    SimResult getSimResult(int robotIndex)
    {
        // Simulate the game and predict the final score
        // Return the last safe position of the robot with the input index

        SimResult result, robotResult;

        // Cycle through the robots
        for (int i = 0; i < robots.size(); ++i)
        {
            auto & robot = robots[i];
            robotResult = getRobotResult(robot);
            result.score += robotResult.score;

            if (i == robotIndex)
            {
                result.lastPos = robotResult.lastPos;
            }
        }

        return result;
    }

    array<int,2> testArrowPosition(array<int, 2> & position, int & bestScore, string & solution, int robotIndex)
    {
        // See if putting an arrow on the  position can improve the score (reset every time)
        SimResult result;
        array<int, 2> lastPosition = { {-1,-1} };
        CELLTYPE finalType = PLATFORM;
        string solutionAddition = "";

        // LEFT
        putCellType(position, LEFTARROW);
        result = getSimResult(robotIndex);
        if (result.score > bestScore)
        {
            bestScore = result.score;
            solutionAddition = to_string(position.at(1)) + " " + to_string(position.at(0)) + " L ";
            lastPosition = result.lastPos;
            finalType = LEFTARROW;
        }
        robots = initRobots;

        // RIGHT
        putCellType(position, RIGHTARROW);
        result = getSimResult(robotIndex);
        if (result.score > bestScore)
        {
            bestScore = result.score;
            solutionAddition = to_string(position.at(1)) + " " + to_string(position.at(0)) + " R ";
            lastPosition = result.lastPos;
            finalType = RIGHTARROW;
        }
        robots = initRobots;

        // UP
        putCellType(position, UPARROW);
        result = getSimResult(robotIndex);
        if (result.score > bestScore)
        {
            bestScore = result.score;
            solutionAddition = to_string(position.at(1)) + " " + to_string(position.at(0)) + " U ";
            lastPosition = result.lastPos;
            finalType = UPARROW;
        }
        robots = initRobots;

        // DOWN
        putCellType(position, DOWNARROW);
        result = getSimResult(robotIndex);
        if (result.score > bestScore)
        {
            bestScore = result.score;
            solutionAddition = to_string(position.at(1)) + " " + to_string(position.at(0)) + " D ";
            lastPosition = result.lastPos;
            finalType = DOWNARROW;
        }
        robots = initRobots;

        // Before leaving, return the board to the state that produced the best score;
        putCellType(position, finalType);
        solution += solutionAddition;

        return lastPosition;
    }
};

int main()
{
    Board board;

    auto start = clock();

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

    // Save the starting robot states
    board.initRobots = board.robots;

    // Get the default score as a starting point
    string solution = "";
    SimResult result = board.getSimResult(0);
    int bestScore = result.score;
    auto currLastPos = result.lastPos;

    // Reset the robots
    board.robots = board.initRobots;

    // Try to maximize the path of each robot
    for (int i = 0; i < robotCount; ++i)
    {
        // Test whether putting an arrow of some sort on the last possible spot in the robot's path can improve the score
        // Keep going until no arrow helps (this is when it returns -1,-1 as the last pos)
        while (currLastPos.at(0) >= 0)
        {
            currLastPos = board.testArrowPosition(currLastPos, bestScore, solution, i);
        }

        currLastPos = board.getSimResult(i + 1).lastPos;
    }



    cerr << "time " << double(clock() - start) / CLOCKS_PER_SEC << endl;
    cerr << bestScore << endl;
    cout << solution << endl;
}