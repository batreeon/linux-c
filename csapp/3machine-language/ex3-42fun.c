struct ELE {
    long v;
    struct ELE *p;
};

/*
// 错误版本，汇编表达的是while循环结构
long fun(struct ELE *ptr) {
    if (!ptr) {
        return 0;
    }
    long result = ptr -> v;
    ptr = ptr -> p;
    return result;
}
*/

long fun(struct ELE *ptr) {
    long result = 0;
    while (ptr) {
        result += ptr -> v;
        ptr = ptr -> p;
    }
    return result;
}