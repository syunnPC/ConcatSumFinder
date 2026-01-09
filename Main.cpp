#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>

#ifdef __SIZEOF_INT128__
using bigint = __int128_t;
// 128ビット整数の出力用ヘルパー（10進数表示）
std::ostream& operator<<(std::ostream& os, bigint n)
{
    if (n == 0) return os << "0";
    std::string s;
    bigint temp = n; // nを破壊しないようにコピー
    while (temp > 0)
    {
        s += (char)('0' + (temp % 10));
        temp /= 10;
    }
    std::reverse(s.begin(), s.end());
    return os << s;
}
#else
using bigint = unsigned long long;
#endif


// 数値を指定された基数(base <= 36)の文字列表現に変換する関数
std::string to_base_string(bigint value, long long base)
{
    if (base < 2 || base > 36) return "?"; // 範囲外対応
    if (value == 0) return "0";

    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result = "";
    bigint temp = value;

    while (temp > 0)
    {
        int remainder = (int)(temp % base);
        result += chars[remainder];
        temp /= base;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// xを計算する関数: x = d(d-1) / 2(N-d)
bigint calculate_x(bigint d, bigint N)
{
    bigint numerator = d * (d - 1);
    bigint denominator = 2 * (N - d);

    if (denominator <= 0) return -1;

    if (numerator % denominator == 0)
    {
        return numerator / denominator;
    }
    return -1;
}

// xからdの下限を探索
bigint find_lower_bound_d(bigint N, bigint target_x)
{
    bigint low = 1, high = N - 1;
    bigint ans = N;

    while (low <= high)
    {
        bigint mid = low + (high - low) / 2;
        bigint lhs = mid * (mid - 1);
        bigint rhs = 2 * target_x * (N - mid);

        if (lhs >= rhs)
        {
            ans = mid;
            high = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }
    return ans;
}

// xからdの上限を探索
bigint find_upper_bound_d(bigint N, bigint target_x)
{
    bigint low = 1, high = N - 1;
    bigint ans = 0;

    while (low <= high)
    {
        bigint mid = low + (high - low) / 2;
        bigint lhs = mid * (mid - 1);
        bigint rhs = 2 * target_x * (N - mid);

        if (lhs <= rhs)
        {
            ans = mid;
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return ans;
}

// べき乗計算
bigint power(bigint base, long long exp)
{
    bigint res = 1;
    while (exp > 0)
    {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
}

int main()
{
    long long b_in, n_in, m_in;
    long long max_count_in = -1;

    std::cout << "基数 b を入力 (2-36): ";
    std::cin >> b_in;
    std::cout << "xの桁数 n を入力: ";
    std::cin >> n_in;
    std::cout << "yの桁数 m を入力: ";
    std::cin >> m_in;

    // フィルタリングオプション
    std::string use_limit;
    bigint user_min_x = 0;
    bigint user_max_x = -1;

    std::cout << "解(x)の範囲指定をしますか？ (y/n): ";
    std::cin >> use_limit;
    if (use_limit == "y" || use_limit == "Y")
    {
        long long min_v, max_v;
        std::cout << "最小値 (0で基数の最小桁): ";
        std::cin >> min_v;
        std::cout << "最大値 (-1で基数の最大桁): ";
        std::cin >> max_v;
        user_min_x = (bigint)min_v;
        user_max_x = (bigint)max_v;
    }

    std::cout << "表示する最大解数 (-1で全て): ";
    std::cin >> max_count_in;

    bigint b = (bigint)b_in;
    bigint N = power(b, m_in);

    bigint min_digit_x = power(b, n_in - 1);
    bigint max_digit_x = power(b, n_in) - 1;

    bigint search_min_x = (user_min_x > min_digit_x) ? user_min_x : min_digit_x;
    bigint search_max_x = max_digit_x;
    if (user_max_x != -1 && user_max_x < max_digit_x)
    {
        search_max_x = user_max_x;
    }

    if (search_min_x > search_max_x)
    {
        std::cout << "\n指定された範囲に解の候補はありません。\n";
        return 0;
    }

    std::cout << "\n探索開始...\n";
    std::cout << "基数(DEC): " << b_in << ", x桁数: " << n_in << ", y桁数: " << m_in << "\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    // ヘッダーの調整
    if (b_in <= 36 && b_in != 10)
    {
        std::cout << "   x (10進)    | x (基数" << std::setw(2) << b_in << ") |   y (10進)    | y (基数" << std::setw(2) << b_in << ") | 差分 d\n";
    }
    else
    {
        std::cout << "   x (10進)    |    y (10進)    | 差分 d \n";
    }
    std::cout << "--------------------------------------------------------------------------------\n";

    bigint start_d = find_lower_bound_d(N, search_min_x);
    bigint end_d = find_upper_bound_d(N, search_max_x);

    if (start_d < 1) start_d = 1;
    if (end_d >= N) end_d = N - 1;

    long long found_count = 0;

    for (bigint d = start_d; d <= end_d; ++d)
    {
        bigint x = calculate_x(d, N);

        if (x != -1)
        {
            bigint y = x + d;

            if (x >= search_min_x && x <= search_max_x && y < N)
            {
                // 出力処理
                if (b_in <= 36 && b_in != 10)
                {
                    std::string x_str = to_base_string(x, b_in);
                    std::string y_str = to_base_string(y, b_in);

                    std::cout << std::setw(14) << x << " | "
                        << std::setw(11) << x_str << " | "
                        << std::setw(14) << y << " | "
                        << std::setw(11) << y_str << " | "
                        << std::setw(6) << d << "\n";
                }
                else
                {
                    std::cout << std::setw(14) << x << " | "
                        << std::setw(14) << y << " | "
                        << std::setw(6) << d << "\n";
                }

                found_count++;
                if (max_count_in != -1 && found_count >= max_count_in)
                {
                    std::cout << "...指定件数に達しました。\n";
                    break;
                }
            }
        }
    }

    if (found_count == 0)
    {
        std::cout << "解は見つかりませんでした。\n";
    }
    else
    {
        std::cout << "合計 " << found_count << " 個の解が見つかりました。\n";
    }

    return 0;
}
