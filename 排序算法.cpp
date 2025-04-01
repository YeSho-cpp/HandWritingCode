#include <bits/stdc++.h>

using namespace std;

/* 选择排序 */
void selectionSort(vector<int> &nums) {
    int n = nums.size();
    // 外循环：未排序区间为 [i, n-1]
    for (int i = 0; i < n - 1; i++) {
        // 内循环：找到未排序区间内的最小元素
        int k = i;
        for (int j = i + 1; j < n; j++) {
            if (nums[j] < nums[k])
                k = j; // 记录最小元素的索引
        }
        // 将该最小元素与未排序区间的首个元素交换
        swap(nums[i], nums[k]);
    }
}


/* 冒泡排序（标志优化）*/
void bubbleSortWithFlag(vector<int> &nums) {
    // 外循环：未排序区间为 [0, i]
    for (int i = nums.size() - 1; i > 0; i--) {
        bool flag = false; // 初始化标志位
        // 内循环：将未排序区间 [0, i] 中的最大元素交换至该区间的最右端
        for (int j = 0; j < i; j++) {
            if (nums[j] > nums[j + 1]) {
                // 交换 nums[j] 与 nums[j + 1]
                // 这里使用了 std::swap() 函数
                swap(nums[j], nums[j + 1]);
                flag = true; // 记录交换元素
            }
        }
        if (!flag)
            break; // 此轮“冒泡”未交换任何元素，直接跳出
    }
}

/* 插入排序 */
void insertionSort(vector<int> &nums) {
    // 外循环：已排序区间为 [0, i-1]
    for (int i = 1; i < nums.size(); i++) {
        int base = nums[i], j = i - 1;
        // 内循环：将 base 插入到已排序区间 [0, i-1] 中的正确位置
        while (j >= 0 && nums[j] > base) {
            nums[j + 1] = nums[j]; // 将 nums[j] 向右移动一位
            j--;
        }
        nums[j + 1] = base; // 将 base 赋值到正确位置
    }
}

/* 哨兵划分 */
int partition(vector<int> &nums, int left, int right) {
    // 以 nums[left] 为基准数
    int i = left, j = right;
    while (i < j) {
        while (i < j && nums[j] >= nums[left])
            j--; // 从右向左找首个小于基准数的元素
        while (i < j && nums[i] <= nums[left])
            i++; // 从左向右找首个大于基准数的元素
        swap(nums[i], nums[j]); // 交换这两个元素
    }
    swap(nums[i], nums[left]); // 将基准数交换至两子数组的分界线
    return i;            // 返回基准数的索引
}


/* 快速排序 */
void quickSort(vector<int> &nums, int left, int right) {
    // 子数组长度为 1 时终止递归
    if (left >= right)
        return;
    // 哨兵划分
    int pivot = partition(nums, left, right);
    // 递归左子数组、右子数组
    quickSort(nums, left, pivot - 1);
    quickSort(nums, pivot + 1, right);
}


/* 合并左子数组和右子数组 */
void merge(vector<int> &nums, int left, int mid, int right) {
    // 左子数组区间为 [left, mid], 右子数组区间为 [mid+1, right]
    // 创建一个临时数组 tmp ，用于存放合并后的结果
    vector<int> tmp(right - left + 1);
    // 初始化左子数组和右子数组的起始索引
    int i = left, j = mid + 1, k = 0;
    // 当左右子数组都还有元素时，进行比较并将较小的元素复制到临时数组中
    while (i <= mid && j <= right) {
        if (nums[i] <= nums[j])
            tmp[k++] = nums[i++];
        else
            tmp[k++] = nums[j++];
    }
    // 将左子数组和右子数组的剩余元素复制到临时数组中
    while (i <= mid) {
        tmp[k++] = nums[i++];
    }
    while (j <= right) {
        tmp[k++] = nums[j++];
    }
    // 将临时数组 tmp 中的元素复制回原数组 nums 的对应区间
    for (k = 0; k < tmp.size(); k++) {
        nums[left + k] = tmp[k];
        
    }
}

/* 归并排序 */
void mergeSort(vector<int> &nums, int left, int right) {
    // 终止条件
    if (left >= right)
        return; // 当子数组长度为 1 时终止递归
    // 划分阶段
    int mid = (left + right) / 2;    // 计算中点
    mergeSort(nums, left, mid);      // 递归左子数组
    mergeSort(nums, mid + 1, right); // 递归右子数组
    // 合并阶段
    merge(nums, left, mid, right);
}

/* 堆排序*/

void siftUp(vector<int>&nums, int i){
    while(i>0){
        int parents=(i-1)/2;
        if(nums[i]>nums[parents]){
            swap(nums[i],nums[parents]);
            i=parents;
        }else{
            break;
        }
    }
}

void heapify(vector<int> &nums, int n, int i) {
    int largest = i; // 初始化最大值索引
    int l = 2 * i + 1; // 左子节点索引
    int r = 2 * i + 2; // 右子节点索引
    // 找出三者中的最大值索引
    if (l < n && nums[l] > nums[largest])
        largest = l;
    if (r < n && nums[r] > nums[largest])
        largest = r;
    // 若最大值索引不是父节点索引，则交换父节点与最大值节点
    if (largest != i) {
        swap(nums[i], nums[largest]);
        // 递归调整子树
        heapify(nums, n, largest);
    }
}

void buildHeap(vector<int> &nums) {
    int n = nums.size();
    // 从最后一个非叶子节点开始，自底向下构建最大堆
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(nums, n, i);
    }
}

void sortArray(vector<int> &nums) {
    buildHeap(nums); // 构建最大堆
    int n = nums.size();
    // 逐步将堆顶元素（最大值）与堆尾元素交换，再调整堆
    for (int i = n - 1; i > 0; i--) {
        swap(nums[0], nums[i]);
        heapify(nums, i, 0);
    }
}


/*计数排序*/

// 计数排序函数
void countingSort(std::vector<int>& arr) {
    // 如果数组为空或只有一个元素，直接返回
    if (arr.size() <= 1) return;
    
    // 找出数组中的最大值和最小值
    int max_val = *std::max_element(arr.begin(), arr.end());
    int min_val = *std::min_element(arr.begin(), arr.end());
    
    // 计算值域范围
    int range = max_val - min_val + 1;
    
    // 创建计数数组和输出数组
    std::vector<int> count(range, 0);
    std::vector<int> output(arr.size());
    
    // 统计每个元素出现的次数
    for (int i = 0; i < arr.size(); i++) {
        count[arr[i] - min_val]++;
    }
    
    // 计算每个元素在输出数组中的位置
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    
    // 构建输出数组
    for (int i = arr.size() - 1; i >= 0; i--) {
        output[count[arr[i] - min_val] - 1] = arr[i];
        count[arr[i] - min_val]--;
    }
    
    // 将排序结果复制回原数组
    for (int i = 0; i < arr.size(); i++) {
        arr[i] = output[i];
    }
}

/*桶排序*/
// 桶排序函数
void bucketSort(std::vector<float>& arr) {
    // 如果数组为空或只有一个元素，直接返回
    if (arr.size() <= 1) return;
    
    // 找出数组中的最大值和最小值
    float max_val = *std::max_element(arr.begin(), arr.end());
    float min_val = *std::min_element(arr.begin(), arr.end());
    
    // 确定桶的数量，这里使用数组大小作为桶的数量
    int bucket_count = arr.size();
    
    // 创建桶（使用list便于插入）
    std::vector<std::list<float>> buckets(bucket_count);
    
    // 计算每个桶的范围
    float range = (max_val - min_val) / bucket_count;
    
    // 将元素放入对应的桶中
    for (int i = 0; i < arr.size(); i++) {
        // 计算元素应该放入哪个桶
        int bucket_index = (arr[i] - min_val) / range;
        
        // 处理边界情况，最大值应该放在最后一个桶
        if (bucket_index == bucket_count) {
            bucket_index--;
        }
        
        buckets[bucket_index].push_back(arr[i]);
    }
    
    // 对每个桶中的元素进行排序
    for (int i = 0; i < bucket_count; i++) {
        buckets[i].sort(); // list自带的排序方法
    }
    
    // 合并所有桶中的元素
    int index = 0;
    for (int i = 0; i < bucket_count; i++) {
        for (float val : buckets[i]) {
            arr[index++] = val;
        }
    }
}


/*基数排序*/

// 对数组按照指定位数进行计数排序
void countingSortByDigit(std::vector<int>& arr, int exp) {
    int n = arr.size();
    std::vector<int> output(n);
    std::vector<int> count(10, 0); // 10个可能的数字（0-9）
    
    // 统计每个数字出现的次数
    for (int i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
    }
    
    // 计算累积计数，确定每个数字的位置
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }
    
    // 从后向前遍历，构建输出数组
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }
    
    // 将排序结果复制回原数组
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// 基数排序函数
void radixSort(std::vector<int>& arr) {
    // 如果数组为空或只有一个元素，直接返回
    if (arr.size() <= 1) return;
    
    // 找出最大值以确定最大位数
    int max_val = *max_element(arr.begin(),arr.end());
    
    // 从最低位开始，对每一位进行计数排序
    for (int exp = 1; max_val / exp > 0; exp *= 10) {
        countingSortByDigit(arr, exp);
    }
}