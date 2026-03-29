# Homework 3: Poisson Image Editing 实验报告

## 1. 算法原理与实现 (Algorithm Implementation)

### 1.1 Poisson Image Editing 原理简述
*(在这里简要描述 Poisson Image Editing 的核心思想，即在保持目标图像背景的同时，通过求解泊松方程，将源图像的梯度场无缝融合到目标图像中。)*

### 1.2 Seamless Cloning (Importing Gradients) 实现细节
*(说明你是如何实现 Seamless Cloning 的。主要包括：)*
- **稀疏矩阵 A 的构建**：解释 `precompute()` 中按照 Laplacian 算子（4, -1, -1, -1, -1）构建稀疏矩阵的过程。
- **预分解优化**：说明为了支持实时拖拽，使用了 `Eigen::SimplicialLDLT` 对矩阵 A 进行仅仅一次的预分解。
- **右端项 B 的构建与求解**：解释在 `solve()` 中如何结合源图的梯度（Guidance field）和目标图的边界条件（Dirichlet boundary）构造向量 B，并快速求解出未知像素点的值。

### 1.3 实时拖拽 (Real-time Dragging) 优化机制
*(解释你是怎么做到流畅实时拖拽的，这是本作业的重点之一：)*
- 每次鼠标拖动时，由于选区大小（$W \times H$）并未改变，**系数矩阵 A 也保持不变**。
- 因此，不需要每帧重新构造和分解矩阵 A。通过 `SeamlessClone::set_offset()` 仅仅更新目标区域的位置偏移量 `offset_x_` 和 `offset_y_`。
- 每帧只需重新构造向量 $B$（因为边界条件变了）并调用预分解好的解算器去 `solve(B)`，极大地提高了计算速度，实现了实时随鼠标移动的无缝融合。

---

## 2. 实验结果 (Results)

*(在这里贴上你的运行截图或动图展示)*

### 2.1 融合效果对比
- **原图展示**：(插入一张 Target 图，一张 Source 图)
- **Paste (直接粘贴) 效果**：(插入直接 Paste 的截图，体现颜色不协调的突兀感)
- **Seamless Cloning 效果**：(插入用 Poisson Editing 融合后的截图，体现颜色的无缝过渡)

### 2.2 实时拖拽演示
- *(如果可以，描述一下你开启 Realtime 选项后，拖拽时的流畅体验，或者插入一张拖拽过程中的截图/GIF)*

---

## 3. 遇到的问题及解决方法 (Challenges & Solutions)

- **性能瓶颈**：初期如果在每一帧都重新初始化 `SeamlessClone` 对象并重新进行矩阵分解，会导致严重的卡顿（报 `solver not initialized` 或极其缓慢）。
- **解决方法**：将矩阵的**预分解 (precompute)** 与 **右端项更新和求解 (solve)** 解耦。增加 `set_offset` 方法，在拖拽期间只更新坐标，复用已有对象的 LDLT 分解结果，完美解决了实时渲染的性能问题。
- **坐标对齐问题**：最初实现时鼠标总是对应原图的左上角，导致交互不符合直觉。通过将 offset 计算改为 `offset_x = mouse_x - W/2`，实现了以鼠标为中心的拖拽交互。

---

## 4. 选做部分 (Optional / Extra Credit) (如有)

*(如果你没有做这部分，直接删除即可)*

### 4.1 混合梯度 (Mixing Gradients)
*(简述实现思路：在构造 guidance field 时，比较源图梯度和目标图梯度的大小，取绝对值较大的那个作为 guidance。附上体现物体透明度融合的截图。)*

### 4.2 复杂多边形边界 (Complex Boundaries)
*(简述如何通过多边形扫描线算法，构建非矩形的 mask，并在构造稀疏矩阵和求解时只处理 mask 内的有效像素。附上自定义多边形融合的截图。)*
