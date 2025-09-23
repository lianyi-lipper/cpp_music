#include "music.h"
using namespace std;
int main(){
    BGM bgm("7.txt");
    bgm.play();
    while (1);
    bgm.stop();
}
