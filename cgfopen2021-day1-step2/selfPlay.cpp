#include "selfPlay.h"
#include "computerMove.h"

void Selfplay(Position position)
{
    // 黒の手番
    int color = 1;

    // 座標
    int z;

    // 探索方法フラグ
    int search;

    for (;;)
    {
        // 黒番、白番ともにUCT探索
        if (color == 1)
        {
            search = kSearchUct; //kSearchPrimitive;
        }
        else
        {
            search = kSearchUct;
        }

        // 次の一手
        z = PlayComputerMove(position, color, search);

        // パスパスなら終局
        if (z == 0 && position.moves > 1 && position.record[position.moves - 2] == 0)
            break;

        // 300手を超えても終局
        if (position.moves > 300)
            break; // too long

        // 手番の色反転
        color = FlipColor(color);
    }

    // SGF形式で棋譜表示
    position.PrintSgf();
}
