// 错误解法
// x * y = p + t*pow(2,w)
// t != 0当且仅当乘法溢出
int tmult_ok(int32_t x, int32_t y) {
    // 错误，这样写，他还是会先按32计算乘积，然后符号拓展到64位
    int64_t p = x * y;
    return p >> 32
}

// 参考答案
int tmult_ok(int32_t x, int32_t y) {
    int64_t p = (int64_t) x*y;
    return p == (int32_t) p;
}