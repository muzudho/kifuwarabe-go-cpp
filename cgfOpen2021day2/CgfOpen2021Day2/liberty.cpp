#include "liberty.h"

extern Position position;

Liberty libertyClass = Liberty();

void Liberty::CountLibertySub(int tz, int color, int* p_liberty, int* p_stone)
{
    // 着手点
    int z;

    // ループ カウンター
    int i;

    position.check_board[tz] = 1; // search flag
    (*p_stone)++;        // number of stone
    for (i = 0; i < 4; i++)
    {
        z = tz + kDir4[i];
        if (position.check_board[z])
            continue;
        if (position.Board[z] == 0)
        {
            position.check_board[z] = 1;
            (*p_liberty)++; // number of liberty
        }
        if (position.Board[z] == color)
            CountLibertySub(z, color, p_liberty, p_stone);
    }
}

void Liberty::CountLiberty(int tz, int* p_liberty, int* p_stone)
{
    int i;
    *p_liberty = *p_stone = 0;
    for (i = 0; i < kBoardMax; i++)
        position.check_board[i] = 0;
    CountLibertySub(tz, position.Board[tz], p_liberty, p_stone);
}
