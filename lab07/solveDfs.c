// DFS maze solver

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Cell.h"
#include "helpers.h"
#include "Maze.h"
#include "Stack.h"

static bool DFSPathCheck(Maze m, Cell start);
static bool setNeighbourState(Maze m, int i, int j, Cell v, Cell **visited, bool **state, Stack s);
static bool validCell(Maze m, Cell c);
static bool checkCell(Cell x, Cell y);

bool solve(Maze m) {
    return DFSPathCheck(m, MazeGetStart(m));
}

// The following code borrows the algorithm provided in the
// course lecture notes and adapts it.
static bool DFSPathCheck(Maze m, Cell start) {

    // Create an initialise cell array to keep track of predecessing cell
    Cell **visited = createCellMatrix(MazeHeight(m), MazeWidth(m));
    bool **state = createBoolMatrix(MazeHeight(m), MazeWidth(m));

    for (int i = 0; i < MazeHeight(m); i++) {
        for (int j = 0; j < MazeWidth(m); j++) {
            Cell x;
            x.row = i, x.col = j;
            if (!MazeIsWall(m, x)) {
                state[i][j] = false;
            }
        }
    }

    bool found = MazeVisit(m, start);
    state[start.row][start.col] = true;
    visited[start.row][start.col] = start;

    Stack s = StackNew();
    Cell v;
    StackPush(s, start);
    while (!found && StackSize(s)) {
        v = StackPop(s);
        // set state of all unvisited neighbours to Visited
        // and set v as predecessor cell
        found = setNeighbourState(m, v.row, v.col, v, visited, state, s);
    }
    StackFree(s);
    if (found) {
        MazeMarkPath(m, v);
        while (!checkCell(v, start)) {
            v = visited[v.row][v.col];
            MazeMarkPath(m, v);
        }
        MazeMarkPath(m, start);
    }

    freeCellMatrix(visited);
    freeBoolMatrix(state);

    return found;
}

// Sets the state of unvisited adjacent cells to visited and sets v as predecessor cell
static bool setNeighbourState(Maze m, int i, int j, Cell v, Cell **visited, bool **state, Stack s) {
    Cell x;
    // Left
    x.row = i;
    x.col = j - 1;
    bool found;
    if (validCell(m, x) && 
            !MazeIsWall(m, x) && state[i][j - 1] == false) {
        found = MazeVisit(m, x);
        state[i][j - 1] = true;
        visited[x.row][x.col] = v;
        StackPush(s, x);
        if (found) {
            MazeMarkPath(m, x);
            return found;
        }
    }
    // Up
    x.row = i - 1;
    x.col = j;
    if (validCell(m, x) && 
            !MazeIsWall(m, x) && state[i - 1][j] == false) {
        found = MazeVisit(m, x);
        state[i - 1][j] = true;
        visited[x.row][x.col] = v;
        StackPush(s, x);
        if (found) {
            MazeMarkPath(m, x);
            return found;
        }
    }
    // Right
    x.row = i;
    x.col = j + 1;
    if (validCell(m, x) && 
            !MazeIsWall(m, x) && state[i][j + 1] == false) {
        found = MazeVisit(m, x);
        state[i][j + 1] = true;
        visited[x.row][x.col] = v;
        StackPush(s, x);
        if (found) {
            MazeMarkPath(m, x);
            return found;
        }
    }
    // Down
    x.row = i + 1;
    x.col = j;
    if (validCell(m, x) && 
            !MazeIsWall(m, x) && state[i + 1][j] == false) {
        found = MazeVisit(m, x);
        state[i + 1][j] = true;
        visited[x.row][x.col] = v;
        StackPush(s, x);
        if (found) {
            MazeMarkPath(m, x);
            return found;
        }
    }
    return found;
}

static bool validCell(Maze m, Cell c) {
    return (
        c.row >= 0 && c.row < MazeHeight(m) && c.col >= 0 && c.col < MazeWidth(m)
    );
}

static bool checkCell(Cell x, Cell y) {
    return (x.row == y.row) && (x.col == y.col);
}