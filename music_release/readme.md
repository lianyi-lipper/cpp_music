### 基于 `Windows API MIDI` 的 `music.h` 使用帮助

调用本头文件，能够在您的 C++ 程序中播放音色良好的自定义音乐。

#### 0.1 软件/语言支持

+   x64 Windows 系统
+   C++

#### 0.2 编译器配置

+   开启 C++11 (`-std=c++11`)
+   连接器命令行 `-lwinmm`

### 1. 如何自定义音乐

#### 简谱格式

在任意文本文档中，可以先输入若干**全局设定**，再输入指定格式的字符串（**音符集**），即可完成创作。

**全局设定（可选）**

在乐谱的开头，可以以 `key=value` 的格式设定全局参数。这些设置将应用于所有轨道，除非被轨道特定设置覆盖。目前支持：

+   `dctn` 或 `delay`：乐曲节奏进行的快慢（一个四分音符的延时，单位：毫秒）。
+   `v` 或 `volume`：全局音量（力度），范围 `0` ~ `127`。
+   `instrument`：全局乐器。默认为 `piano`。

例如：

```
v=100
dctn=500
instrument=flute
```

如果不行指定，程序将使用默认值。当程序读到第一个不符合 `key=value` 格式的行时，就会将其以及之后的内容作为**音符集**进行解析。

#### 多轨音序 (Multi-track Sequencing)

本库支持最多 16 个轨道（Channel 0 到 15）的并发播放，允许您创作复杂的、多声部的乐曲。

**轨道特定设置**

除了全局设置，您还可以在乐谱的头部或主体部分为单个轨道指定乐器和音量。语法为：

`轨道号.属性 = 值`

+   `轨道号`: `0` 到 `15` 的整数。
+   `属性`: `instrument` 或 `volume`。

例如：

```
# 全局设定
instrument = piano
volume = 110

# 轨道 0 (右手) 使用长笛
0.instrument = flute
0.volume = 120

# 轨道 1 (左手) 使用贝斯
1.instrument = acousticbass
1.volume = 100
```

**播放块 (Playback Blocks)**

乐谱由一个或多个**播放块**组成，块与块之间用**空行**隔开。在一个块中，每一行音符会被分配给一个独立的轨道（从轨道 0 开始），并同时播放。

**示例：**

```
# --- 块 1 ---
# 轨道 0 (钢琴) 和轨道 1 (贝斯) 同时播放
0.instrument = piano
1.instrument = acousticbass
4_ 4_ 4_ 4_
2,^ 2,^ 2,^ 2,^

# --- 块 2 (空行分隔) ---
# 只有轨道 0 (钢琴) 在播放
5_ 5_ 5_ 5_

# --- 块 3 (空行分隔) ---
# 轨道 1 的乐器被更换为弦乐
1.instrument = stringensemble1
4_ 4_ 4_ 4_
2,^ 2,^ 2,^ 2,^
```

在这个例子中：
1.  第一块包含两行音符，它们将分别在轨道 0 (钢琴) 和轨道 1 (贝斯) 上同时播放。
2.  第二块只有一行，因此只有轨道 0 (钢琴) 会播放。
3.  第三块在播放前将轨道 1 的乐器换成了弦乐，然后再次在轨道 0 和 1 上同时播放。

#### 音符集格式规范

任意两个音符/和弦间应用空格隔开。事实上，播放音符的判定**总**会在空格处执行。

高/低/升音号必须紧跟在音符后面，但这些符号内部的顺序不限。

延/分音号与音符间的顺序不限，但延/分音号内部将从左到右判读。和弦的延/分音号建议放在和弦外面。

| 字符 | 说明 | 使用示例 |
| :--- | :--- | :--- |
| `1~7` | 音符 | 如 `1 2 3 4 5` 表示播放 `do re mi fa sol` 五个音符 |
| `0` | 休止符 | 略 |
| `,` | 低八度 | 每个符号使音符降低一个八度。如 `1,` `2,,` 等 |
| `^` | 高八度 | 每个符号使音符升高一个八度。如 `1^` `2^^` 等 |
| `#` | 升半音 | 向上半个音高。如 `4#` |
| `-` | 延音 | 每个延音号为音符额外增加一个四分音符的时长 |
| `_` | 分音 | 每个分音号将使音符的时长减半 |
| `.` | 附点 | 每个附点号将使音符的时长增半（×1.5） |
| `[]` ` {}` | 和弦 | 中括号或大括号内的音符将一次性演奏 |
| `|` | 小节线 | 程序将忽略这个符号 |
| `v` | 力度 | 设置此后音符的音量（`0`~`127`）。如 `v100` `5v127` |

**在 v1.1/v1.2 版本中**，相继添加了 `*` `%` `&` 分别将音符的延时缩减为原先的三分之一，五分之一和七分之一。

**在 v1.3 版本中**，添加了动态音量控制功能。

#### 动态音量与速度控制

除了通过全局或轨道设置来指定音量和速度外，还可以在乐谱中动态调整它们。

1.  **序列音量/速度**：
    *   `v<value>`: 改变从该点开始之后所有音符的默认音量。示例: `1 1 5 5 v70 4 4 3 3`
    *   `dctn=<value>`: 改变从该点开始之后的速度。示例: `1 1 [dctn=250] 2 2` (注意：此指令必须单独占据一行或在块之间，不能与音符混用)

2.  **单音音量**：
    *   将 `v` 和数字跟在某个音符（及其所有修饰符）之后，可以仅改变该音符的音量，而不影响后续音符的默认音量。
    *   示例：`4v127 4v100 4v80`
    *   此方法优先级高于序列音量。例如：`v70 5 5 6v127 5`，音符 `6` 的音量是 `127`，而它前后的 `5` 音符音量都是 `70`。

### 2. 头文件解析

#### 2.1 `MusicPlayer` 类

音乐播放器。将连接相关的 `API` 进行音乐播放操作。

+   `void playList(MusicList &m)`: 播放一个 `MusicList` 对象中加载的乐谱。
+   `void setDelay(int _dctn)`: 设置全局的四分音符延时（毫秒）。
+   `void setInstrument(int channel, int instrument_id)`: 为指定轨道设置乐器。
+   `void setVolume(int channel, int _vol)`: 为指定轨道设置音量（`0`~`127`）。

#### 2.2 `MusicList` 类

歌曲。可以将若干行音符整合，也可从文件中读取音符。

+   `MusicList(string fileName = "")`: 构造函数，可选择从文件中直接加载乐谱。
+   `void readFile(string fileName)`: 从文件中读取乐谱，解析全局和轨道设置。
+   `void add(string s)`: 插入一行简谱格式的音符。
+   `void clear()`: 清空歌曲和所有设置。
+   `void setDelay(int _dctn)`: 设置默认的四分音符延时（毫秒）。

#### 2.3 `BGM` 类

使用多线程，可使程序在运行同时播放歌曲。

+   `BGM(string name, int volume = 127)`: 构造函数，从文件 `name` 中读取音符集并设定初始全局音量。
+   `void setMusic(string s)`: 从文件中读取音符集并绑定。
+   `void play()`: 在新线程中开始循环播放此音乐。
+   `void stop()`: 停止播放此音乐。

### 3. 程序使用范例

#### 3.1 播放指定乐曲

```cpp
#include "music.h"
#include <iostream>

int main(){
    MusicPlayer player;
    // 直接通过构造函数加载乐谱文件
    MusicList music("test_multitrack.txt");
    player.playList(music);
    return 0;
}
```

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
    // 构造BGM对象，直接加载乐谱并设置全局音量
    BGM bgm("test_multitrack.txt", 100);
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

### 附录：可用乐器列表

<details>
<summary>点击展开/折叠</summary>

| 名称 (Name) | ID |
| :--- | :--- |
| piano / acousticgrand | 0 |
| brightacoustic | 1 |
| electricgrand | 2 |
| honkytonk | 3 |
| electricpiano1 | 4 |
| electricpiano2 | 5 |
| harpsichord | 6 |
| clav | 7 |
| celesta | 8 |
| glockenspiel | 9 |
| musicbox | 10 |
| vibraphone | 11 |
| marimba | 12 |
| xylophone | 13 |
| tubularbells | 14 |
| dulcimer | 15 |
| drawbarorgan | 16 |
| percussiveorgan | 17 |
| rockorgan | 18 |
| churchorgan | 19 |
| reedorgan | 20 |
| accordion | 21 |
| harmonica | 22 |
| concertina | 23 |
| acousticguitarnylon | 24 |
| guitar / acousticguitarsteel | 25 |
| electricguitarjazz | 26 |
| electricguitarclean | 27 |
| electricguitarmuted | 28 |
| overdrivenguitar | 29 |
| distortedguitar | 30 |
| guitarharmonics | 31 |
| acousticbass | 32 |
| electricbassfinger | 33 |
| electricbasspick | 34 |
| fretlessbass | 35 |
| slapbass1 | 36 |
| slapbass2 | 37 |
| synthbass1 | 38 |
| synthbass2 | 39 |
| violin | 40 |
| viola | 41 |
| cello | 42 |
| contrabass | 43 |
| tremolostrings | 44 |
| pizzicatostrings | 45 |
| orchestralharp | 46 |
| timpani | 47 |
| stringensemble1 | 48 |
| stringensemble2 | 49 |
| synthstrings1 | 50 |
| synthstrings2 | 51 |
| choiraahs | 52 |
| voiceoohs | 53 |
| synthvoice | 54 |
| orchestrahit | 55 |
| trumpet | 56 |
| trombone | 57 |
| tuba | 58 |
| mutedtrumpet | 59 |
| frenchhorn | 60 |
| brasssection | 61 |
| synthbrass1 | 62 |
| synthbrass2 | 63 |
| sopranosax | 64 |
| altosax | 65 |
| tenorsax | 66 |
| baritonesax | 67 |
| oboe | 68 |
| englishhorn | 69 |
| bassoon | 70 |
| clarinet | 71 |
| piccolo | 72 |
| flute | 73 |
| recorder | 74 |
| panflute | 75 |
| blownbottle | 76 |
| shakuhachi | 77 |
| whistle | 78 |
| ocarina | 79 |
| lead1square | 80 |
| lead2sawtooth | 81 |
| lead3calliope | 82 |
| lead4chiff | 83 |
| lead5charang | 84 |
| lead6voice | 85 |
| lead7fifths | 86 |
| lead8basslead | 87 |
| pad1newage | 88 |
| pad2warm | 89 |
| pad3polysynth | 90 |
| pad4choir | 91 |
| pad5bowed | 92 |
| pad6metallic | 93 |
| pad7halo | 94 |
| pad8sweep | 95 |
| fx1rain | 96 |

</details>