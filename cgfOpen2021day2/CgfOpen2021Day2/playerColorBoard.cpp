#include "playerColorBoard.h"

bool PlayerColorBoard::IsEmpty(int z) {
    return played_color[z] == 0;
}

void PlayerColorBoard::SetColor(int z, int color) {
    played_color[z] = color;
}
