项目：情绪日记本
一、提供的接口
1. 获取某月所有情绪数据（给 wyx）
函数名：getMonthData(year, month)
参数：
`year`：年份，如 2026
`month`：月份，1-12
返回值：`QVector<QPair<QString, QVector3D>>`
```cpp
#include "mainwindow.h"
MainWindow mainWindow = new MainWindow();
auto data = mainWindow->getMonthData(2026, 5);
for (auto &item : data) {
    QString date = item.first;      // "2026-05-14"
    QVector3D pad = item.second;    // (P, A, D)
    int p = pad.x();    // 愉悦度 0-255
    int a = pad.y();    // 唤醒度 0-255
    int d = pad.z();    // 支配度 0-255
}
```
用途：
日历画布：用(p, a, d)作为 RGB 填充格子
情绪曲线：画 P、A、D 三条折线图
注意：没有记录的日期不会返回，需要处理空缺。

2. 获取某一天的情绪数据（给 wjq）

函数名：`getEmotionByDate(date)`
参数：
`date`：日期字符串，格式"YYYY-MM-DD"
返回值：`QVector3D`（x=P，y=A，z=D）
示例：
```cpp
#include "mainwindow.h"

MainWindow mainWindow = new MainWindow();
QVector3D emotion = mainWindow->getEmotionByDate("2026-05-14");

int p = emotion.x();    // 愉悦度
int a = emotion.y();    // 唤醒度
int d = emotion.z();    // 支配度
```
注意：如果该日期没有记录，返回(128, 128, 128)中性灰。

二、数据库说明
文件名：`emotions.db`（SQLite）
位置：程序运行目录下（第一次运行自动生成）
表名：`daily_emotions`
表结构：
| 字段 | 类型 | 说明 |
|------|------|------|
| date | TEXT | 日期（主键） |
| p | INTEGER | 愉悦度 0-255 |
| a | INTEGER | 唤醒度 0-255 |
| d | INTEGER | 支配度 0-255 |
| diary | TEXT | 日记内容（HTML格式） |

表名：`capsule_shown`
表结构：
| 字段 | 类型 | 说明 |
|------|------|------|
| last_shown_date | TEXT | 已弹窗日期 |


三、功能模块说明
模块一：情绪记录（RecordWindow）
1. 功能概述
用户可以通过三个滑块分别调整愉悦度（P）、唤醒度（A）、支配度（D）的数值，实时预览颜色变化，并将情绪数据保存到数据库中。
2. 使用方式
在主窗口中点击 “记录每日情绪🖊” 按钮，打开记录窗口。
3. 功能说明
- 日期选择：可选择任意日期进行情绪记录或修改。
- 滑块调节：
  - 愉悦度（P）：难过 ← → 开心
  - 唤醒度（A）：平静 ← → 兴奋
  - 支配度（D）：无力 ← → 掌控
- 实时预览：
  - 颜色预览框：以 (P, A, D) 作为 RGB 显示颜色
  - 数值标签：实时显示当前三个维度的数值
- 保存功能：点击“保存今日情绪”按钮，将数据写入数据库（存在则覆盖）。
4. 示例代码
```cpp
#include "recordemotions.h"

RecordWindow recordWin = new RecordWindow();
recordWin->show();
```
5. 常见问题
| 保存失败 | 检查数据库是否初始化成功，程序目录是否有写入权限 |
| 切换日期后滑块不变 | 确保该日期已有保存记录，否则滑块会重置为 128 |
| 预览颜色不更新 | 检查 `onSliderChanged()` 是否正确连接信号 |

模块二：情绪日历（CalendarWidget）
1. 功能概述
以日历表格形式直观展示某一月份每天的情绪状态。每个日期格子根据当天的情绪值 (P, A, D) 填充不同的背景颜色，实现“一眼看透整月心情”的效果。
2. 使用方式
在主窗口中点击 “查看月度情绪📅” 按钮，打开情绪日历窗口。
3. 功能说明
- 颜色映射：每个日期格子的背景色由当天记录的 (P, A, D) 三值作为 RGB 颜色通道生成。
  - 开心的一天 → 红色成分高（P 值高），颜色偏暖
  - 平静的一天 → 蓝色成分高（D 值高），颜色偏冷
- 日期切换：日历控件支持上下月翻页，切换月份后自动刷新对应月份的颜色。
- 无数据日期：若某天没有情绪记录，格子显示为白色背景、黑色文字。
- 文字颜色自适应：根据背景色的亮度自动选择黑色或白色文字，确保可读性。
4. 接口依赖
依赖 `MainWindow::getMonthData(year, month)` 接口获取数据。
5. 示例代码
```cpp
#include "calendarview.h"

MainWindow mw = new MainWindow();
CalendarWidget calendar = new CalendarWidget(mw);
calendar->show();
```
6. 常见问题
| 日历格子全是白色 | 确认该月份已有保存的情绪记录 |
| 切换月份后颜色不变 | 检查 `currentPageChanged` 信号是否正确连接 |
| 文字看不清 | 系统已自动根据背景亮度调整黑白文字 |
| 颜色和预期不符 | 颜色由 (P, A, D) 直接映射为 (R, G, B) |

模块三：情绪趋势图（LineChartsWindow）
1. 功能概述
以折线图形式展示某一月份内情绪维度的变化趋势，支持单独查看或对比展示 P、A、D 三条曲线，帮助用户分析情绪波动规律。
2. 使用方式
在主窗口中点击 “查看月度趋势📈” 按钮，打开趋势图窗口。
3. 功能说明
- 维度选择：
  - 愉悦度 (P) - 难过 ← → 开心
  - 唤醒度 (A) - 平静 ← → 兴奋
  - 支配度 (D) - 无力 ← → 掌控
  - 全部维度 (P+A+D) - 对比显示
- 时间筛选：可选择年份和月份，动态刷新对应月份的情绪数据。
- 图形绘制：
  - 横轴为日期，纵轴为情绪值（0–255）
  - 曲线圆滑连接，数据点高亮显示并标注数值
  - 支持图例说明，区分不同情绪维度
  - 辅助网格线和坐标轴标签
- 数据缺失处理：若某月无情绪记录，窗口显示提示信息。
4. 接口依赖
依赖 `MainWindow::getMonthData(year, month)` 接口获取数据。
5. 示例代码
```cpp
#include "linecharts.h"

MainWindow mw = new MainWindow();
LineChartsWindow chartWindow = new LineChartsWindow(mw);
chartWindow->show();
```
6. 常见问题
| 图形不显示曲线 | 确认该月份已有保存的情绪记录 |
| 切换维度无变化 | 检查 `onDimensionChanged` 是否正确触发 |
| 窗口卡顿或绘制异常 | 尝试调整窗口大小，或减少同时显示的维度 |
| 日期标签重叠 | 系统会自动调整标签显示间隔 |

模块五：日记功能
1.	功能概述
支持编辑和仅查看全部日期图文日记，方便用户记录当日的所思所想、补充过去的思绪回忆。
2.	使用方式
“记录每日情绪”：可任选某日期编辑日记，调整字体、字号，选择粗体、斜体、下划线、横线，亦可插入图片。插入图片后文字保持插入前的格式设置。无保存按钮，窗口关闭时自动保存。
“查看月度情绪”：点击日期，激活弹窗或日记只读富文本窗口。若无日记切弹窗选择写日记，则打开日记编辑窗口，使用方式见上。
3.	功能说明
·编辑模式（dDiarywWindow）：从“记录每日情绪”进入，实现富文本编辑，有字体、字号、粗体、斜体、下划线、横线、插入图片等众多功能可选。
·只读模式（DiaryvViewWindow）：从“查看月度情绪”或“时光胶囊”进入，不可编辑，展示日记全部文本格式。
·自动保存：无需手动保存，窗口关闭自动保存。
·空日记处理：检测到内容为空时不保存。
4.	接口依赖
MainWindow::getDatabase() — 获取数据库连接
5.	示例代码
```cpp
#include "diarywindow.h"
#include "diaryviewwindow.h"

// 编辑模式
DiaryWindow *diaryWin = new DiaryWindow();
diaryWin->setDate("2026-05-24");
diaryWin->show();

// 只读模式
DiaryViewWindow *viewWin = new DiaryViewWindow(parent);
viewWin->loadDiary("2026-05-24", htmlContent);
viewWin->show();
```
6.	常见问题
·图片不显示：检查图片路径是否有效
·格式丢失：自动保存仅在正常关闭情况下执行，确认关闭前程序未崩溃
·日记无法编辑：检查是否从“记录每日情绪”进入

模块六：时光胶囊
1.	功能概述
弹窗显示上月同日期情绪色块和日记，勾起用户回忆，便于用户对比跨时空情绪思绪变化，形成情感连接。
2.	使用方式
当日第一次“进入”Mainwindow三个选项中任意一个，若上月同日期有日记则弹窗显示时光胶囊和当日情绪色块（无情绪记录时为中性灰），默认选项打开只读日记窗口，另一选项仅关闭弹窗。每日最多弹窗一次。
3.	功能说明
·触发时机：每天首次打开任意功能且上月同天有非空日记时
·弹窗内容：情绪色块、上月日期、提示文字、选项（默认选项打开只读日记）
·去重机制：每天最多弹窗一次
4.	接口依赖
MainWindow::getEmotionByDate(date) — 获取情绪色块
5.	示例代码
/
6.	常见问题
·每天多次弹窗：检查mainwindow.cpp中“query.exec("DELETE FROM capsule_shown");是否注释掉”
·不弹窗：检查上月是否有该日期、上月该日期是否有非空日记、当日是否由mainwindow【重新】进入三个功能之一
·情绪色块颜色有误：检查getEmotionByDate返回值（无记录时返回中性灰）

四、如何集成到项目
步骤1：复制文件
把以下文件复制到 Qt 项目文件夹：
- `mainwindow.h` / `mainwindow.cpp`
- `recordemotions.h` / `recordemotions.cpp`
- `calendarview.h` / `calendarview.cpp`
- `linecharts.h` / `linecharts.cpp`
diarywindow.h / diarywindow.cpp
diaryviewwindow.h / diaryviewwindow.cpp
步骤2：修改 .pro 文件
确保 `.pro` 文件里有：
```
QT += core widgets sql
CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    recordemotions.cpp \
    calendarview.cpp \
linecharts.cpp
diarywindow.cpp
diaryviewindow.cpp

HEADERS += \
    mainwindow.h \
    recordemotions.h \
    calendarview.h \
linecharts.h
diarywindow.h
diaryviewwindow.cpp
```
步骤3：包含头文件
```cpp
#include "mainwindow.h"
#include "recordemotions.h"
#include "calendarview.h"
#include "linecharts.h"
#include “diarywindow.h”
#include “diaryviewwindow.h”
```
步骤4：创建对象并调用
```cpp
MainWindow mainWindow = new MainWindow();
mainWindow->show();
```

五、模块关系总结
情绪记录（RecordWindow）：保存每天的情绪值；数据库读写；日常记录心情
情绪日历（CalendarWidget）：按月显示情绪颜色分布；`getMonthData()`；回顾整月情绪变化
情绪趋势图（LineChartsWindow）：按月绘制情绪折线图；`getMonthData()`；分析情绪波动趋势
日记编辑（DiaryWindow）：编辑图文日记；记录当下思绪；数据库读写
日记查看（DiaryViewWindow）：只读查看日记，回顾过往所思所想
时光胶囊（checkCapsule）：自动弹窗；getEmotionByDate()；跨时空情感联结
六个模块共享同一个 SQLite 数据库（`emotions.db`），数据互通，互不影响。

六、注意事项
1. 第一次运行：需要先运行一次程序（会自动创建 `emotions.db` 和表）
2. 数据为空：如果还没保存任何情绪，`getMonthData` 返回空列表
3. 编译报错：检查 `.pro` 文件是否有 `sql` 模块
4. 头文件路径：确保所有 `.h` 文件在项目目录中
5. 窗口管理：各子窗口使用 `QPointer` 管理，支持重复打开而不重复创建

七、测试代码
```cpp
#include "mainwindow.h"
#include "diarywindow.h"
#include "diaryviewwindow.h"
#include <QDebug>

int main(int argc, char argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow = new MainWindow();
    mainWindow->show();

    // 测试获取某天数据
    QVector3D emotion = mainWindow->getEmotionByDate("2026-05-16");
    qDebug() << "P:" << emotion.x() << "A:" << emotion.y() << "D:" << emotion.z();

    // 测试获取整月数据
    auto data = mainWindow->getMonthData(2026, 5);
qDebug() << "本月有" << data.size() << "天有记录";

// 测试日记编辑
DiaryWindow *diaryWin = new DiaryWindow();
diaryWin->setDate("2026-05-16");
diaryWin->show();

// 测试日记查看
DiaryViewWindow *viewWin = new DiaryViewWindow(mainWindow);
viewWin->loadDiary("2026-05-16", "<p>测试日记内容</p>");
viewWin->show();

    return a.exec();
}
```

八、常见问题汇总
| 编译报错 QSqlDatabase | `.pro` 文件里加 `sql` |
| 编译报错 QVector3D | 添加 `#include <QVector3D>` |
| 数据库打不开 | 检查程序运行目录是否有写入权限 |
| 接口返回空数据 | 先运行程序保存几条数据 |
| 日历格子全白 | 确认该月份已有情绪记录 |
| 趋势图无曲线 | 确认该月份已有情绪记录 |
| 日记格式丢失 | 确认正常关闭窗口，崩溃可能导致未保存 |
| 时光胶囊每天弹多次 | 检查 capsule_shown 表是否正常写入 |
