#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

// 浮点数比较容差
#define EPSILON 1e-5

// 物品结构体
typedef struct {
    int id;         // 物品编号
    int weight;     // 物品重量
    float value;    // 物品价值
    float ratio;    // 价值重量比，用于贪心算法
    int selected;   // 是否被选中
} Item;

// 函数声明
void generateItems(Item *items, int n);
void printItems(Item *items, int n);
float bruteForce(Item *items, int n, int capacity);
float dynamicProgramming(Item *items, int n, int capacity);
float greedy(Item *items, int n, int capacity);
float backtrack(Item *items, int n, int capacity);
float bound(Item *items, int n, int capacity, int i, float weight, float value);
void backtrack_helper(Item *items, int n, int capacity, int i, int *current_choice,
                    int *best_choice, float *best_value, float weight, float value);
void printSelectedItems(Item *items, int n);

int main() {
    int test_n = 30;    // 物品数量
    int test_capacity = 1000;  // 背包容量

    Item *items = (Item *)malloc(test_n * sizeof(Item));
    if (items == NULL) {
        printf("内存分配失败！\n");
        return 1;
    }

    srand(time(NULL));  // 初始化随机数种子

    // 生成随机物品
    generateItems(items, test_n);

    printf("物品数量: %d, 背包容量: %d\n", test_n, test_capacity);
    printf("物品列表:\n");
    printItems(items, test_n);

    printf("\n===== 蛮力法 =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    clock_t start_time = clock();
    float brute_value = bruteForce(items, test_n, test_capacity);
    clock_t end_time = clock();
    double brute_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("最大价值: %.2f\n", brute_value);
    printf("执行时间: %.2f s\n", brute_time);
    printSelectedItems(items, test_n);


    printf("\n===== 动态规划法 =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    start_time = clock();
    float dp_value = dynamicProgramming(items, test_n, test_capacity);
    end_time = clock();
    double dp_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("最大价值: %.2f\n", dp_value);
    printf("执行时间: %.2f s\n", dp_time);
    printSelectedItems(items, test_n);

    printf("\n===== 贪心法 =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    start_time = clock();
    float greedy_value = greedy(items, test_n, test_capacity);
    end_time = clock();
    double greedy_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("最大价值: %.2f\n", greedy_value);
    printf("执行时间: %.2f s\n", greedy_time);
    printSelectedItems(items, test_n);

    printf("\n===== 回溯法 =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    start_time = clock();
    float backtrack_value = backtrack(items, test_n, test_capacity);
    end_time = clock();
    double backtrack_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("最大价值: %.2f\n", backtrack_value);
    printf("执行时间: %.2f s\n", backtrack_time);
    printSelectedItems(items, test_n);

    free(items);
    return 0;
}

// 生成随机数
void generateItems(Item *items, int n) {
    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));  // 使用当前时间作为随机种子
        initialized = 1;
    }

    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;

        // 生成随机重量（1 - 100）
        items[i].weight = (rand() % 100) + 1;

        // 生成随机价值（100.00 - 1000.00）
        double value_random = (double)rand() / RAND_MAX;
        items[i].value = 100.0 + value_random * 900.0;

        items[i].selected = 0;
    }
}

// 打印物品列表
void printItems(Item *items, int n) {
    printf("编号\t重量\t价值\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%.2f\n", items[i].id, items[i].weight, items[i].value);
    }
}

// 蛮力法
float bruteForce(Item *items, int n, int capacity) {
    float max_value = 0.0;
    int best_choice = 0;

    // 枚举所有可能的物品组合
    for (int i = 0; i < (1 << n); i++) {
        float total_weight = 0;
        float total_value = 0.0;

        for (int j = 0; j < n; j++) {
            if (i & (1 << j)) {
                total_weight += items[j].weight;
                total_value += items[j].value;
            }
        }

        if (total_weight <= capacity + EPSILON && total_value > max_value) {
            max_value = total_value;
            best_choice = i;
        }
    }

    // 标记选中的物品
    for (int j = 0; j < n; j++) {
        if (best_choice & (1 << j)) {
            items[j].selected = 1;
        }
    }

    return max_value;
}

// 动态规划法
float dynamicProgramming(Item *items, int n, int capacity) {
    // 创建一维dp数组
    float *dp = (float *)calloc(capacity + 1, sizeof(float));
    if (dp == NULL) {
        printf("动态规划内存分配失败！\n");
        return 0.0;
    }

    // 使用二维数组记录选择路径
    int **choice = (int **)malloc(n * sizeof(int *));
    if (choice == NULL) {
        printf("选择路径内存分配失败！\n");
        free(dp);
        return 0.0;
    }

    for (int i = 0; i < n; i++) {
        choice[i] = (int *)calloc(capacity + 1, sizeof(int));
        if (choice[i] == NULL) {
            printf("选择路径内存分配失败！\n");
            // 释放已分配的内存
            for (int j = 0; j < i; j++) {
                free(choice[j]);
            }
            free(choice);
            free(dp);
            return 0.0;
        }
    }

    // 动态规划循环
    for (int i = 0; i < n; i++) {
        int weight_i = items[i].weight;
        float value_i = items[i].value;

        // 从大到小遍历背包容量
        for (int w = capacity; w >= weight_i; w--) {
            float new_value = dp[w - weight_i] + value_i;
            if (dp[w] < new_value) {
                dp[w] = new_value;
                choice[i][w] = 1;  // 记录选择
            }
        }
    }

    float best_value = dp[capacity];

    // 回溯找出选中的物品
    int w = capacity;
    for (int i = n - 1; i >= 0; i--) {
        if (choice[i][w] == 1) {
            items[i].selected = 1;
            w -= items[i].weight;
        }
    }

    // 释放内存
    for (int i = 0; i < n; i++) {
        free(choice[i]);
    }
    free(choice);
    free(dp);

    return best_value;
}

// 贪心法
float greedy(Item *items, int n, int capacity) {
    // 复制物品数组
    Item *items_copy = (Item *)malloc(n * sizeof(Item));
    if (items_copy == NULL) {
        printf("贪心法内存分配失败！\n");
        return 0.0;
    }

    memcpy(items_copy, items, n * sizeof(Item));

    // 计算每个物品的价值重量比
    for (int i = 0; i < n; i++) {
        items_copy[i].ratio = items_copy[i].value / items_copy[i].weight;
    }

    // 按价值重量比排序
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (items_copy[j].ratio < items_copy[j + 1].ratio) {
                Item temp = items_copy[j];
                items_copy[j] = items_copy[j + 1];
                items_copy[j + 1] = temp;
            }
        }
    }

    int current_weight = 0;
    float total_value = 0.0;

    // 贪心选择物品
    for (int i = 0; i < n; i++) {
        if (current_weight + items_copy[i].weight <= capacity + EPSILON) {
            // 找到原始数组中的对应物品
            for (int j = 0; j < n; j++) {
                if (items[j].id == items_copy[i].id) {
                    items[j].selected = 1;
                    break;
                }
            }

            current_weight += items_copy[i].weight;
            total_value += items_copy[i].value;
        }
    }

    free(items_copy);
    return total_value;
}

// 上界函数
float bound(Item *items, int n, int capacity, int i, float weight, float value) {
    float bound_value = value;
    float bound_weight = weight;

    for (int j = i; j < n; j++) {
        // 使用容差比较浮点数
        if (bound_weight + items[j].weight <= capacity + EPSILON) {
            bound_weight += items[j].weight;
            bound_value += items[j].value;
        } else {
            bound_value += (capacity - bound_weight) * items[j].ratio;
            break;
        }
    }
    return bound_value;
}

// 回溯递归函数
void backtrack_helper(Item *items, int n, int capacity, int i, int *current_choice,
                    int *best_choice, float *best_value, float weight, float value) {
    // 递归终止条件：添加重量合法性检查
    if (i == n) {
        if (weight <= capacity + EPSILON && value > *best_value) {
            *best_value = value;
            memcpy(best_choice, current_choice, n * sizeof(int));
        }
        return;
    }

    // 剪枝优化：添加上界检查
    float current_bound = bound(items, n, capacity, i, weight, value);
    if (weight > capacity + EPSILON || current_bound <= *best_value + EPSILON) {
        return;
    }

    // 选择当前物品（使用容差比较）
    if (weight + items[i].weight <= capacity + EPSILON) {
        current_choice[i] = 1;
        backtrack_helper(items, n, capacity, i + 1, current_choice, best_choice,
                         best_value, weight + items[i].weight, value + items[i].value);
    }

    // 不选择当前物品
    current_choice[i] = 0;
    backtrack_helper(items, n, capacity, i + 1, current_choice, best_choice,
                    best_value, weight, value);
}

// 主回溯函数
float backtrack(Item *items, int n, int capacity) {
    // 计算价值重量比
    for (int i = 0; i < n; i++) {
        items[i].ratio = items[i].value / items[i].weight;
    }

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (items[j].ratio < items[j + 1].ratio) {
                Item temp = items[j];
                items[j] = items[j + 1];
                items[j + 1] = temp;
            }
        }
    }

    // 初始化变量
    float best_value = 0.0;
    int *current_choice = (int *)calloc(n, sizeof(int));
    int *best_choice = (int *)calloc(n, sizeof(int));

    if (!current_choice || !best_choice) {
        printf("内存分配失败！\n");
        free(current_choice);
        free(best_choice);
        return 0.0;
    }

    // 开始回溯
    backtrack_helper(items, n, capacity, 0, current_choice, best_choice,
                    &best_value, 0.0, 0.0);

    // 标记选中的物品
    float total_weight = 0.0;
    for (int i = 0; i < n; i++) {
        items[i].selected = best_choice[i];
        if (best_choice[i]) {
            total_weight += items[i].weight;
        }
    }

    free(current_choice);
    free(best_choice);
    return best_value;
}

// 打印选中的物品
void printSelectedItems(Item *items, int n) {
    int total_weight = 0;
    float total_value = 0.0;
    int selected_count = 0;

    for (int i = 0; i < n; i++) {
        if (items[i].selected) {
            selected_count++;
        }
    }

    if (selected_count == 0) {
        printf("没有选择任何物品\n");
    } else {
        printf("选择的物品:\n");
        printf("编号\t重量\t价值\n");

        for (int i = 0; i < n; i++) {
            if (items[i].selected) {
                printf("%d\t%df\t%.2f\n", items[i].id, items[i].weight, items[i].value);
                total_weight += items[i].weight;
                total_value += items[i].value;
            }
        }
    }

    printf("总重量: %d\n", total_weight);
    printf("总价值: %.2f\n", total_value);
}
