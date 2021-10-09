#include "hashCode.h"

void HashCode::MakeHashboard()
{
    int z, i;
    for (z = 0; z < kBoardMax; z++)
    {
        // std::cerr << "[" << z << "]=";
        for (i = 0; i < kHashKinds; i++)
        {
            hashboard[z][i] = Rand64();
            //    PrtCode64(hashboard[z][i]);
            // std::cerr << ",";
        }
        // std::cerr << std::endl;
    }
}

void HashCode::HashPass()
{
    hashcode = ~hashcode;
}

void HashCode::HashXor(int z, int color)
{
    // 指定座標、指定色のハッシュの 2進数の 0と1 を反転させたものを ハッシュコードに記憶させている？
    hashcode ^= hashboard[z][color];
}
