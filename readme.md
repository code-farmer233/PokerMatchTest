# 纸牌程序设计【需求一】v2 设计说明

## 一、项目概述

本项目基于 Cocos2d-x 3.17 实现一个简化纸牌匹配玩法，覆盖题目要求的三项核心能力：

1. 手牌区翻牌替换：点击备用牌堆堆顶牌，卡牌平移到手牌区顶部并替换当前顶部牌。
2. 桌面牌匹配：点击桌面牌时，若其点数与手牌区顶部牌相差 1，则卡牌平移到手牌区顶部并成为新的顶部牌，花色不参与判断。
3. 回退功能：每次翻牌或匹配都会记录操作历史，点击回退按钮后按倒序执行反向平移动画并恢复数据状态，直到没有可回退记录。

项目使用题目资源包中的 `card_general.png`、`number`、`suits` 资源拼装牌面，设计分辨率设置为 `1080 x 2080`，窗口创建参数与需求文档保持一致。

## 二、目录结构

```text
cocos2d/
├── Classes/
│   ├── AppDelegate.h/.cpp          # Cocos2d-x 应用入口与分辨率配置
│   ├── CardModel.h                 # 卡牌数据模型
│   ├── CardView.h/.cpp             # 单张卡牌视图、点击、牌面资源拼装
│   ├── CardGameController.h/.cpp   # 玩法控制器：匹配、翻牌、回退
│   └── PokerScene.h/.cpp           # 场景装配、测试牌局布局、UI 按钮
├── proj.win32/
│   └── main.cpp                    # Windows 入口
├── res/
│   └── res/                        # 题目提供的图片资源
├── CMakeLists.txt                  # CMake 工程配置
└── readme.md
```

## 三、架构设计

项目采用轻量 MVC 思路，将数据、视图和玩法控制分离。

### 1. Model：CardModel

`CardModel` 只描述卡牌数据，不负责展示和动画：

- `id`：卡牌唯一标识。
- `rank`：点数，A 为 1，J/Q/K 为 11/12/13。
- `suit`：花色。
- `area`：当前所属区域，包含桌面牌区、备用牌区、手牌顶部区。
- `faceUp`：是否翻开。

点数显示、花色命名等工具函数也放在模型头文件中，便于 `CardView` 根据模型选择资源。

### 2. View：CardView

`CardView` 继承自 `cocos2d::Node`，负责单张卡牌的视觉表现：

- 使用 `card_general.png` 作为牌底。
- 根据点数和花色加载 `number/big_*`、`number/small_*`、`suits/*`。
- 处理触摸命中，并通过回调把点击事件交给控制器。
- 提供 `refresh()` 刷新翻开状态。
- 提供简单的无效点击反馈动画。

`CardView` 不判断能否匹配，也不直接修改游戏流程。

### 3. Controller：CardGameController

`CardGameController` 是玩法核心，负责三类状态管理：

- 桌面牌集合：所有可匹配的桌面牌。
- 备用牌堆：只有当前堆顶牌可点击，堆顶牌被翻出后自动露出下一张。
- 手牌顶部牌：当前用于匹配的底牌。

核心接口：

```cpp
void addTableCard(CardView* card);
void addStockCard(CardView* card);
void setInitialWasteCard(CardView* card);
void handleCardClicked(CardView* card);
void undo();
```

匹配规则集中在 `canMatch()`：

```cpp
std::abs(tableCard->getModel().rank - _wasteTop->getModel().rank) == 1
```

该规则不读取花色，因此符合“无花色限制”的要求。

### 4. Scene：PokerScene

`PokerScene` 负责场景装配：

- 添加背景、标题、区域提示和回退按钮。
- 初始化题目示例牌局。
- 将点击事件从 `CardView` 转发到 `CardGameController`。

示例牌局只作为验证数据，不包含业务特判。继续新增桌面牌或备用牌时，只需要调用对应的 `addTableCard()` 或 `addStockCard()`。

## 四、回退设计

当前版本使用轻量操作记录 `MoveRecord` 实现回退。每次卡牌从桌面牌区或备用牌区移动到手牌顶部前，记录移动前状态：

- 操作类型：桌面牌匹配或备用牌翻牌。
- 被移动卡牌。
- 移动前位置。
- 移动前所属区域。
- 移动前翻开状态。
- 移动前手牌顶部牌。
- 移动前备用牌堆顶牌。
- 移动前 z-order。

回退时按历史栈倒序取出记录，恢复数据状态，并让卡牌 `MoveTo` 回到原位置。恢复完成后重新刷新备用牌堆顶和输入状态。

这种设计能覆盖当前需求，也方便未来演进为完整 Command 模式。如果新增“翻开覆盖牌”“发多张牌”“洗牌”等复杂操作，可以把每种操作拆成独立命令类：

```cpp
class ICommand {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
};
```

每个命令保存自己的恢复数据，控制器只维护命令栈。

## 五、核心流程

### 1. 手牌区翻牌替换

1. 用户点击备用牌堆堆顶牌。
2. `CardView` 触发点击回调。
3. `CardGameController::handleCardClicked()` 判断该牌属于 `CardArea::Stock` 且是 `_stockTop`。
4. 记录 `MoveRecord`。
5. 卡牌区域切换为 `Waste`，播放 `MoveTo` 到手牌顶部。
6. 动画结束后，该牌成为新的 `_wasteTop`，备用牌堆刷新下一张堆顶。

### 2. 桌面牌匹配

1. 用户点击桌面翻开牌。
2. 控制器判断该牌属于 `CardArea::Table`。
3. 调用 `canMatch()` 检查点数是否与手牌顶部牌相差 1。
4. 匹配成功则记录历史并播放平移动画。
5. 动画结束后，该牌成为新的手牌顶部牌。

### 3. 多步回退

1. 用户点击 `Undo`。
2. 控制器弹出最近一次 `MoveRecord`。
3. 恢复手牌顶部牌、备用牌堆顶牌、卡牌区域和翻开状态。
4. 卡牌反向 `MoveTo` 回原位置。
5. 如果历史栈仍有记录，回退按钮继续可用。

## 六、扩展说明

### 新增桌面牌

在 `PokerScene::init()` 的 `tableCards` 数据中新增一项：

```cpp
{id, rank, CardSuit::Diamond, Vec2(x, y), zOrder}
```

创建后调用 `_controller.addTableCard(card)` 即可纳入匹配逻辑。

### 新增备用牌

在 `stockCards` 数据中新增一项：

```cpp
{id, rank, CardSuit::Heart, kStockPosition, zOrder}
```

创建后调用 `_controller.addStockCard(card)`。后加入的牌会作为更上层备用牌，当前堆顶被翻出后，下一个仍在 `Stock` 区域的牌会自动成为新堆顶。

### 新增匹配规则

修改 `CardGameController::canMatch()` 即可。例如增加 A 与 K 首尾相接规则：

```cpp
int diff = std::abs(a - b);
return diff == 1 || diff == 12;
```

由于规则集中在控制器中，视图层无需修改。

### 新增回退类型

如果新增操作只涉及移动一张牌，可以扩展 `MoveRecord::ActionType`。如果操作涉及多张牌或复杂状态，建议新增 `ICommand` 命令接口，让每种操作独立实现 `execute()` 和 `undo()`。

## 七、验证场景

当前场景初始化了以下测试数据：

- 手牌顶部牌：`club 4`
- 桌面牌：`diamond 3`、`spade 2`
- 备用牌堆：`heart A` 位于堆顶，另有一张备用牌用于验证堆顶刷新

推荐测试路径：

1. 点击 `diamond 3`，应移动到手牌顶部。
2. 点击备用牌堆堆顶 `heart A`，应移动到手牌顶部。
3. 点击 `spade 2`，应移动到手牌顶部。
4. 连续点击 `Undo` 三次，卡牌应按 `spade 2 -> heart A -> diamond 3` 的顺序回到原位置。
5. 当没有历史记录时，`Undo` 按钮应置灰不可用。

## 八、编译运行说明

本项目按 Cocos2d-x 3.17 源码工程组织。若使用 CMake 构建，需要设置：

```powershell
cmake -S D:\cocos2d -B D:\cocos2d\build `
  -G "Visual Studio 17 2022" -A Win32 `
  -DCOCOS2DX_ROOT_PATH=D:\cocos2d_tools\cocos2d-x-3.17.2\cocos2d-x-cocos2d-x-3.17.2
```

生成成功后打开 `D:\cocos2d\build\PokerMatchTest.sln`，选择 `Debug | Win32` 编译运行。
