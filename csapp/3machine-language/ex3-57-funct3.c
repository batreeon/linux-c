double funct3(int *ap, double b, long c, float *dp)
{
    float x = *dp;
    if (b - *ap <= 0)
    {
        return (double)(*dp + *dp + c);
    }
    return (double)(*dp * c);
}

// 更清楚的版本
double funct3(int *ap, double b, long c, float *dp)
{
    int a = *ap;
    float d = *dp;
    if (d < a)
    {
        return d * c;
    }
    return 2 * d +c;
}