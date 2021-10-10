#include "playerColorBoard.h"

bool PlayerColorBoard::IsEmpty(int z) {
    return _played_color[z] == 0;
}

void PlayerColorBoard::SetColor(int z, int color) {
    _played_color[z] = color;
}

int PlayerColorBoard::GetColor(int z) {
    return _played_color[z];
}