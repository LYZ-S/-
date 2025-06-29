#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

// �������Ƚ��ݲ�
#define EPSILON 1e-5

// ��Ʒ�ṹ��
typedef struct {
    int id;         // ��Ʒ���
    int weight;     // ��Ʒ����
    float value;    // ��Ʒ��ֵ
    float ratio;    // ��ֵ�����ȣ�����̰���㷨
    int selected;   // �Ƿ�ѡ��
} Item;

// ��������
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
    int test_n = 30;    // ��Ʒ����
    int test_capacity = 1000;  // ��������

    Item *items = (Item *)malloc(test_n * sizeof(Item));
    if (items == NULL) {
        printf("�ڴ����ʧ�ܣ�\n");
        return 1;
    }

    srand(time(NULL));  // ��ʼ�����������

    // ���������Ʒ
    generateItems(items, test_n);

    printf("��Ʒ����: %d, ��������: %d\n", test_n, test_capacity);
    printf("��Ʒ�б�:\n");
    printItems(items, test_n);

    printf("\n===== ������ =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    clock_t start_time = clock();
    float brute_value = bruteForce(items, test_n, test_capacity);
    clock_t end_time = clock();
    double brute_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("����ֵ: %.2f\n", brute_value);
    printf("ִ��ʱ��: %.2f s\n", brute_time);
    printSelectedItems(items, test_n);


    printf("\n===== ��̬�滮�� =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    start_time = clock();
    float dp_value = dynamicProgramming(items, test_n, test_capacity);
    end_time = clock();
    double dp_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("����ֵ: %.2f\n", dp_value);
    printf("ִ��ʱ��: %.2f s\n", dp_time);
    printSelectedItems(items, test_n);

    printf("\n===== ̰�ķ� =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    start_time = clock();
    float greedy_value = greedy(items, test_n, test_capacity);
    end_time = clock();
    double greedy_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("����ֵ: %.2f\n", greedy_value);
    printf("ִ��ʱ��: %.2f s\n", greedy_time);
    printSelectedItems(items, test_n);

    printf("\n===== ���ݷ� =====\n");
    for (int i = 0; i < test_n; i++) {
        items[i].selected = 0;
    }

    start_time = clock();
    float backtrack_value = backtrack(items, test_n, test_capacity);
    end_time = clock();
    double backtrack_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("����ֵ: %.2f\n", backtrack_value);
    printf("ִ��ʱ��: %.2f s\n", backtrack_time);
    printSelectedItems(items, test_n);

    free(items);
    return 0;
}

// ���������
void generateItems(Item *items, int n) {
    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));  // ʹ�õ�ǰʱ����Ϊ�������
        initialized = 1;
    }

    for (int i = 0; i < n; i++) {
        items[i].id = i + 1;

        // �������������1 - 100��
        items[i].weight = (rand() % 100) + 1;

        // ���������ֵ��100.00 - 1000.00��
        double value_random = (double)rand() / RAND_MAX;
        items[i].value = 100.0 + value_random * 900.0;

        items[i].selected = 0;
    }
}

// ��ӡ��Ʒ�б�
void printItems(Item *items, int n) {
    printf("���\t����\t��ֵ\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%.2f\n", items[i].id, items[i].weight, items[i].value);
    }
}

// ������
float bruteForce(Item *items, int n, int capacity) {
    float max_value = 0.0;
    int best_choice = 0;

    // ö�����п��ܵ���Ʒ���
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

    // ���ѡ�е���Ʒ
    for (int j = 0; j < n; j++) {
        if (best_choice & (1 << j)) {
            items[j].selected = 1;
        }
    }

    return max_value;
}

// ��̬�滮��
float dynamicProgramming(Item *items, int n, int capacity) {
    // ����һάdp����
    float *dp = (float *)calloc(capacity + 1, sizeof(float));
    if (dp == NULL) {
        printf("��̬�滮�ڴ����ʧ�ܣ�\n");
        return 0.0;
    }

    // ʹ�ö�ά�����¼ѡ��·��
    int **choice = (int **)malloc(n * sizeof(int *));
    if (choice == NULL) {
        printf("ѡ��·���ڴ����ʧ�ܣ�\n");
        free(dp);
        return 0.0;
    }

    for (int i = 0; i < n; i++) {
        choice[i] = (int *)calloc(capacity + 1, sizeof(int));
        if (choice[i] == NULL) {
            printf("ѡ��·���ڴ����ʧ�ܣ�\n");
            // �ͷ��ѷ�����ڴ�
            for (int j = 0; j < i; j++) {
                free(choice[j]);
            }
            free(choice);
            free(dp);
            return 0.0;
        }
    }

    // ��̬�滮ѭ��
    for (int i = 0; i < n; i++) {
        int weight_i = items[i].weight;
        float value_i = items[i].value;

        // �Ӵ�С������������
        for (int w = capacity; w >= weight_i; w--) {
            float new_value = dp[w - weight_i] + value_i;
            if (dp[w] < new_value) {
                dp[w] = new_value;
                choice[i][w] = 1;  // ��¼ѡ��
            }
        }
    }

    float best_value = dp[capacity];

    // �����ҳ�ѡ�е���Ʒ
    int w = capacity;
    for (int i = n - 1; i >= 0; i--) {
        if (choice[i][w] == 1) {
            items[i].selected = 1;
            w -= items[i].weight;
        }
    }

    // �ͷ��ڴ�
    for (int i = 0; i < n; i++) {
        free(choice[i]);
    }
    free(choice);
    free(dp);

    return best_value;
}

// ̰�ķ�
float greedy(Item *items, int n, int capacity) {
    // ������Ʒ����
    Item *items_copy = (Item *)malloc(n * sizeof(Item));
    if (items_copy == NULL) {
        printf("̰�ķ��ڴ����ʧ�ܣ�\n");
        return 0.0;
    }

    memcpy(items_copy, items, n * sizeof(Item));

    // ����ÿ����Ʒ�ļ�ֵ������
    for (int i = 0; i < n; i++) {
        items_copy[i].ratio = items_copy[i].value / items_copy[i].weight;
    }

    // ����ֵ����������
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

    // ̰��ѡ����Ʒ
    for (int i = 0; i < n; i++) {
        if (current_weight + items_copy[i].weight <= capacity + EPSILON) {
            // �ҵ�ԭʼ�����еĶ�Ӧ��Ʒ
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

// �Ͻ纯��
float bound(Item *items, int n, int capacity, int i, float weight, float value) {
    float bound_value = value;
    float bound_weight = weight;

    for (int j = i; j < n; j++) {
        // ʹ���ݲ�Ƚϸ�����
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

// ���ݵݹ麯��
void backtrack_helper(Item *items, int n, int capacity, int i, int *current_choice,
                    int *best_choice, float *best_value, float weight, float value) {
    // �ݹ���ֹ��������������Ϸ��Լ��
    if (i == n) {
        if (weight <= capacity + EPSILON && value > *best_value) {
            *best_value = value;
            memcpy(best_choice, current_choice, n * sizeof(int));
        }
        return;
    }

    // ��֦�Ż�������Ͻ���
    float current_bound = bound(items, n, capacity, i, weight, value);
    if (weight > capacity + EPSILON || current_bound <= *best_value + EPSILON) {
        return;
    }

    // ѡ��ǰ��Ʒ��ʹ���ݲ�Ƚϣ�
    if (weight + items[i].weight <= capacity + EPSILON) {
        current_choice[i] = 1;
        backtrack_helper(items, n, capacity, i + 1, current_choice, best_choice,
                         best_value, weight + items[i].weight, value + items[i].value);
    }

    // ��ѡ��ǰ��Ʒ
    current_choice[i] = 0;
    backtrack_helper(items, n, capacity, i + 1, current_choice, best_choice,
                    best_value, weight, value);
}

// �����ݺ���
float backtrack(Item *items, int n, int capacity) {
    // �����ֵ������
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

    // ��ʼ������
    float best_value = 0.0;
    int *current_choice = (int *)calloc(n, sizeof(int));
    int *best_choice = (int *)calloc(n, sizeof(int));

    if (!current_choice || !best_choice) {
        printf("�ڴ����ʧ�ܣ�\n");
        free(current_choice);
        free(best_choice);
        return 0.0;
    }

    // ��ʼ����
    backtrack_helper(items, n, capacity, 0, current_choice, best_choice,
                    &best_value, 0.0, 0.0);

    // ���ѡ�е���Ʒ
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

// ��ӡѡ�е���Ʒ
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
        printf("û��ѡ���κ���Ʒ\n");
    } else {
        printf("ѡ�����Ʒ:\n");
        printf("���\t����\t��ֵ\n");

        for (int i = 0; i < n; i++) {
            if (items[i].selected) {
                printf("%d\t%df\t%.2f\n", items[i].id, items[i].weight, items[i].value);
                total_weight += items[i].weight;
                total_value += items[i].value;
            }
        }
    }

    printf("������: %d\n", total_weight);
    printf("�ܼ�ֵ: %.2f\n", total_value);
}
