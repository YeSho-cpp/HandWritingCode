

#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
using namespace std;


// 基础版本 - 二倍均值法
class RedPacket {
private:
    double totalAmount;    // 总金额
    int totalNumber;       // 总人数
    double minAmount;      // 最小金额，例如0.01元

public:
    RedPacket(double amount, int number, double min = 0.01) : 
        totalAmount(amount), totalNumber(number), minAmount(min) {}
    
    vector<double> divide() {
        vector<double> result;
        double remainAmount = totalAmount;
        int remainNumber = totalNumber;
        
        // 随机数生成器
        random_device rd;
        mt19937 gen(rd());
        
        for (int i = 0; i < totalNumber - 1; i++) {
            // 计算当前最大可用金额
            double maxAmount = (remainAmount / remainNumber) * 2;
            
            // 生成随机金额
            uniform_real_distribution<> dis(minAmount, maxAmount);
            double amount = dis(gen);
            
            // 保留两位小数
            amount = round(amount * 100) / 100;
            
            result.push_back(amount);
            remainAmount -= amount;
            remainNumber--;
        }
        
        // 最后一个人获得剩余金额
        result.push_back(round(remainAmount * 100) / 100);
        
        return result;
    }
};

// 假设总金额100元，总人数10人，最小金额0.01元

// 第1次抢红包
// 剩余金额：100元
// 剩余人数：10人
// 人均金额：100/10 = 10元
// 最大可抢金额：10 * 2 = 20元
// 随机范围：[0.01, 20]
// 第2次抢红包（假设第1次抢了15元）
// 剩余金额：85元
// 剩余人数：9人
// 人均金额：85/9 ≈ 9.44元
// 最大可抢金额：9.44 * 2 ≈ 18.88元
// 随机范围：[0.01, 18.88]


// 为什么是2倍？
// 假设剩余金额为M，剩余人数为N
// 平均值为 M/N
// 如果随机范围是[0, 2M/N]
// 则随机数的期望值是 (0 + 2M/N)/2 = M/N
// 这保证了每次随机的期望值等于人均金额


// 线段切割法


// 把总金额想象成一条线段，在这条线段上随机切n-1刀，将线段分成n份，每份的长度就是每个红包的金额。

// 总金额线段: 
// 0 |-----------------------------------------| 100
//            ↑        ↑     ↑      ↑
//            切点1    切点2  切点3   切点4

// 切割后金额: 
// |-------|---------|------|--------|---------|
//   红包1    红包2     红包3   红包4    红包5

class RedPacketAdvanced {
private:
    double totalAmount;
    int totalNumber;
    double minAmount;
    
    // 生成切割点
    vector<double> generateCutPoints() {
        vector<double> points;
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0, totalAmount);
        
        // 生成n-1个切割点
        for (int i = 0; i < totalNumber - 1; i++) {
            points.push_back(dis(gen));
        }
        
        // 添加起点和终点
        points.push_back(0);
        points.push_back(totalAmount);
        
        // 排序切割点
        sort(points.begin(), points.end());
        
        return points;
    }

public:
    RedPacketAdvanced(double amount, int number, double min = 0.01) : 
        totalAmount(amount), totalNumber(number), minAmount(min) {}
    
    vector<double> divide() {
        vector<double> result;
        vector<double> points = generateCutPoints();
        
        // 计算相邻切割点之间的距离作为红包金额
        for (int i = 1; i < points.size(); i++) {
            double amount = points[i] - points[i-1];
            
            // 确保每个红包至少有最小金额
            if (amount < minAmount) {
                amount = minAmount;
            }
            
            result.push_back(round(amount * 100) / 100);
        }
        
        return result;
    }
};