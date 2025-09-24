### 基于 `Windows API MIDI` 的 `music.h` 使用帮助

调用本头文件，能够在您的 c++ 程序中播放音色良好的自定义音乐。

#### 0.1 软件/语言支持

+ x64 Windows 系统
+ C++

#### 0.2 编译器配置

+ 开启 C++11 `-std=c++11`
+ 连接器命令行 `-lwinmm`

### 1. 如何自定义音乐

#### 简谱格式

在任意文本文档中，可以先输入若干**全局设定**，再输入指定格式的字符串（**音符集**），即可完成创作。

**全局设定（可选）**

在乐谱的开头，可以以 `key=value` 的格式设定全局参数。目前支持：

+ `dctn` 或 `delay`：乐曲节奏进行的快慢（一个四分音符的延时，毫秒）。
+ `v` 或 `volume`：全局音量（力度），范围 `0` ~ `127`。
+ `instrument`：全局乐器。默认为 `piano`。

例如：
```
v=100
dctn=500
instrument=flute
```
如果不行指定，程序将使用默认值。当程序读到第一个不符合 `key=value` 格式的行时，就会将其以及之后的内容作为**音符集**进行解析。

**在 v1.1 版本中，加入了左右手谱功能，允许程序同时播放两重旋律。此时，音符集部分将自动以两行为一组，每组为同时演奏的两重旋律。使用空行将强制打断多重旋律判定。**

例如：

```
1 2 3 4 5 5 4 3 2 1
1^- 2^- 3^ 4^- 5^--

1 1 5 5 6 6 5

1^- 2^- 3^ 4^- 5^--
```

前两行音符将同时播放，此后，后两行音符将依次播放。


**音符集格式规范**

任意两个音符/和弦间应用空格隔开。事实上，播放音符的判定**总**会在空格处执行。

高/低/升音号必须紧跟在音符后面，但这些符号内部的顺序不限。

延/分音号与音符间的顺序不限，但延/分音号内部将从左到右判读。和弦的延/分音号建议放在和弦外面。

| 字符  | 说明   | 使用示例                                               |
| ----- | ------ | ------------------------------------------------------ |
| `1~7` | 音符   | 如 `1 2 3 4 5` 表示播放 `do re mi fa sol` 五个音符     |
| `0`   | 休止符 | 略                                                     |
| `,`   | 低音   | 即音符向下附点，最多可以有三个低音号。如 `1,` `2,,` 等 |
| `^`   | 高音   | 类比低音                                               |
| `#`   | 升音   | 向上半音高。如 `4#`                                    |
| `-`   | 延音   | 每个延音号将为音符额外多出一个四分音符的延时           |
| `_`   | 分音   | 每个分音号将使音符的延时减半                           |
| `.`   | 附点   | 每个附点号将使音符的延时增半（×1.5）                   |
| `[]` ` {}` | 和弦   | 中括号或大括号内的音符将一次性演奏                             |
| `|`   | 小节线 | 程序将忽略这个符号                                     |
| `v`   | 力度   | 设置此后音符的音量（`0`~`127`）。如 `v100` `5v127`    |

**在 v1.1/v1.2 版本中**，相继添加了 `*` `%` `&` 分别将音符的延时缩减为原先的三分之一，五分之一和七分之一。

**在 v1.3 版本中**，添加了动态音量控制功能。

**动态音量控制**

除了通过全局设定 `v=<value>` 指定整体音量外，还可以在乐谱中动态调整音量。

1.  **序列音量**：使用 `v` 紧跟一个数字（`0`~`127`）来改变从该点开始之后所有音符的默认音量。
    *   示例：`1 1 5 5 v70 4 4 3 3 2 2 1`
    *   在上述例子中，前四个音符 `1 1 5 5` 将使用全局或上一个序列设定的音量，而从 `4` 开始的所有音符，音量都将是 `70`。

2.  **单音音量**：将 `v` 和数字跟在某个音符（及其所有修饰符）之后，可以仅改变该音符的音量，而不影响后续音符的默认音量。
    *   示例：`4v127 4v100 4v80`
    *   在上述例子中，三个 `4` 音符将分别以 `127`, `100`, `80` 的力度演奏。
    *   此方法优先级高于序列音量。例如：`v70 5 5 6v127 5`，音符 `6` 的音量是 `127`，而它前后的 `5` 音符音量都是 `70`。

**动态乐谱控制**

在乐谱中，还可以通过特定指令动态修改乐器和速度。

1.  **动态乐器**：使用 `[instrument=<name>]` 来改变当前乐器。
    *   示例：`1 2 [instrument=flute] 3 4`
    *   在上述例子中，音符 `1` `2` 将使用全局或上一个乐器，而从 `3` 开始，将使用 `flute` 乐器。
    *   关于可用的乐器名称，请参阅附录。

2.  **动态速度**：使用 `[dctn=<value>]` 来改变当前速度。
    *   示例：`1 2 [dctn=250] 3 4`
    *   在上述例子中，音符 `1` `2` 将使用全局或上一个速度，而从 `3` 开始，四分音符的延时将变为 `250` 毫秒。

### 2. 头文件解析

#### 2.1 `MusicPlayer` 类

音乐播放器。将连接相关的 `API` 进行音乐播放操作。

+ `void setVolume(int _vol)` 设置音量（`0x00` 至 `0x7f`）
+ `void setDelay(int _dctn)` 设置四分音符延时（毫秒）
+ `void play(string s1, string s2 = "")` 播放一或两行**简谱格式**的音符。若提供了 `s2`，将与 `s1` 同时播放。
+ `void playList(MusicList &m)` 播放歌曲

#### 2.2 `MusicList` 类

歌曲。可以将若干行音符整合，也可从文件中读取音符。

+ `MusicList(string fileName = "")` 构造函数，可选择从文件中直接加载乐谱。
+ `void setDelay(int _dctn)` 设置四分音符延时（毫秒）——优先于播放器设置的延时
+ `void add(string s)` 插入一行简谱格式的音符
+ `void clear()` 清空歌曲
+ `void readFile(string s)` 从文件中读取音符集

#### 2.3 `BGM` 类

使用多线程，可使程序在运行同时播放歌曲。

+ `BGM(string name, int volume = 0x7f)` 构造函数，从文件 `name` 中读取音符集并设定初始音量。
+ `void setMusic(string s)` 从文件中读取音符集并绑定
+ `void play()` 开始循环播放此音乐
+ `void stop()` 停止播放此音乐

### 3. 程序使用范例

#### 3.1 播放指定乐曲

```cpp
#include "music.h"
#include <iostream>

int main(){
	MusicPlayer player;
	// 直接通过构造函数加载乐谱文件
	MusicList music("skycity.txt");
	player.playList(music);
	return 0;
}
```

### 附录：可用乐器列表

<details>
<summary>点击展开/折叠</summary>

| 名称 (Name) |
| :--- |
| piano / acousticgrand |
| brightacoustic |
| electricgrand |
| honkytonk |
| electricpiano1 |
| electricpiano2 |
| harpsichord |
| clav |
| celesta |
| glockenspiel |
| musicbox |
| vibraphone |
| marimba |
| xylophone |
| tubularbells |
| dulcimer |
| drawbarorgan |
| percussiveorgan |
| rockorgan |
| churchorgan |
| reedorgan |
| accordion |
| harmonica |
| concertina |
| guitar / acousticguitarsteel |
| acousticguitarnylon |
| electricguitarjazz |
| electricguitarclean |
| electricguitarmuted |
| overdrivenguitar |
| distortedguitar |
| guitarharmonics |
| acousticbass |
| electricbassfinger |
| electricbasspick |
| fretlessbass |
| slapbass1 |
| slapbass2 |
| synthbass1 |
| synthbass2 |
| violin |
| viola |
| cello |
| contrabass |
| tremolostrings |
| pizzicatostrings |
| orchestralharp |
| timpani |
| stringensemble1 |
| stringensemble2 |
| synthstrings1 |
| synthstrings2 |
| choiraahs |
| voiceoohs |
| synthvoice |
| orchestrahit |
| trumpet |
| trombone |
| tuba |
| mutedtrumpet |
| frenchhorn |
| brasssection |
| synthbrass1 |
| synthbrass2 |
| sopranosax |
| altosax |
| tenorsax |
| baritonesax |
| oboe |
| englishhorn |
| bassoon |
| clarinet |
| piccolo |
| flute |
| recorder |
| panflute |
| blownbottle |
| shakuhachi |
| whistle |
| ocarina |
| lead1square |
| lead2sawtooth |
| lead3calliope |
| lead4chiff |
| lead5charang |
| lead6voice |
| lead7fifths |
| lead8basslead |
| pad1newage |
| pad2warm |
| pad3polysynth |
| pad4choir |
| pad5bowed |
| pad6metallic |
| pad7halo |
| pad8sweep |
| fx1rain |

</details>

#### 3.2 BGM播放

```cpp
#include "music.h"
#include <iostream>
#include <conio.h> // for getch()

void some_other_functions(){
	std::cout << "BGM is playing in the background." << std::endl;
    std::cout << "The main program can do other things." << std::endl;
	std::cout << "Press any key to stop the BGM and exit." << std::endl;
	getch();
}

int main(){
    // 构造BGM对象，直接加载乐谱并设置音量
	BGM bgm("skycity.txt", 100);
	bgm.play(); // 在新线程中开始播放音乐

    // 主线程可以继续执行其他任务
	some_other_functions();

    // 在程序退出前，优雅地停止BGM
	std::cout << "Stopping BGM..." << std::endl;
	bgm.stop();
    std::cout << "BGM stopped." << std::endl;
	return 0;
}
```
