#include <iostream>

int main() {
    int x = 10;

    // 単純なif文（波括弧なし）
    if (x > 5)
        std::cout << "x is greater than 5" << std::endl;

    // if-else文（波括弧なし）
    if (x < 20)
        std::cout << "x is less than 20" << std::endl;
    else
        std::cout << "x is 20 or greater" << std::endl;

    // else if チェーン
    if (x < 5)
        std::cout << "small" << std::endl;
    else if (x < 15)
        std::cout << "medium" << std::endl;
    else
        std::cout << "large" << std::endl;

    // while文（波括弧なし）
    int i = 0;
    while (i < 3)
        std::cout << "i = " << i++ << std::endl;

    // for文（波括弧なし）
    for (int j = 0; j < 3; j++)
        std::cout << "j = " << j << std::endl;

    // do-while文（波括弧なし）
    int k = 0;
    do
        std::cout << "k = " << k++ << std::endl;
    while (k < 3);

    // ネストされたif文（波括弧なし）
    if (x > 0)
        if (x < 100)
            std::cout << "x is between 0 and 100" << std::endl;

    // 既に波括弧がある場合（変更しない）
    if (x > 0) {
        std::cout << "x is positive" << std::endl;
        std::cout << "This already has braces" << std::endl;
    }

    return 0;
}
