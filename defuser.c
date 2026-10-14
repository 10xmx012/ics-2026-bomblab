#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t low32 = (uint32_t)challenge;
    uint32_t high32 = (uint32_t)(challenge >> 32);
    uint32_t middle32 = (uint32_t)(challenge >> 16);

    uint32_t a = (low32 + UINT32_C(4) * high32)
                 ^ UINT32_C(0x2D73);

    uint32_t b = (middle32 ^ UINT32_C(0xA6BF))
                 + UINT32_C(8) * high32;

    uint32_t c = (UINT32_C(5) * b + low32)
                 ^ UINT32_C(0xE6A6);

    values[0] = c ^ a;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;

    values[0] =
        (uint32_t)((challenge >> 27) & UINT64_C(255))
        * UINT32_C(2) + UINT32_C(10);

    for (uint32_t i = 1; i < 6; i++) {
        unsigned int shift = 15 + 5 * (i - 1);

        uint32_t part =
            (uint32_t)((challenge >> shift) & UINT64_C(63));

        values[i] =
            UINT32_C(5) * (values[i - 1] ^ (part + i));
    }

    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    uint32_t index = (uint32_t)challenge & UINT32_C(7);
    uint32_t x = (uint32_t)(challenge >> 3)
                 ^ (uint32_t)(challenge >> 32);
    uint32_t result = 0;

    switch (index) {
        case 0:
            result = UINT32_C(5) * x + UINT32_C(0x34990);
            break;
        case 1:
            result = UINT32_C(3) * x + UINT32_C(0xB4B);
            break;
        case 2:
            result = (x ^ UINT32_C(0x733C)) + (x >> 1);
            break;
        case 3:
            result = (UINT32_C(4) * x) ^ UINT32_C(0xEE2);
            break;
        case 4:
            result = (UINT32_C(4) * x) ^ UINT32_C(0x5000);
            break;
        case 5:
            result = UINT32_C(5) * x + UINT32_C(0x42CCF);
            break;
        case 6:
            result = UINT32_C(3) * x + UINT32_C(0x56CC);
            break;
        case 7:
            result = (x ^ UINT32_C(0xCA25)) + (x >> 2);
            break;
    }

    values[0] = index;
    values[1] = result;

    return write_answer(out, cap, values, count);
}

static uint32_t phase4_walk_solution(
    uint32_t target,
    uint32_t low,
    uint32_t high,
    uint32_t key
) {
    uint32_t mid = low + ((high - low) >> 1);

    if (target == mid) {
        return (mid + UINT32_C(86)) ^ key;
    }

    uint32_t saved = mid + key;
    uint32_t child;

    if (target < mid) {
        child = phase4_walk_solution(
            target, low, mid - UINT32_C(1), key
        );

        return (child * UINT32_C(5))
               ^ saved ^ UINT32_C(12);
    } else {
        child = phase4_walk_solution(
            target, mid + UINT32_C(1), high, key
        );

        return (child * UINT32_C(5))
               ^ saved ^ UINT32_C(11);
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t target =
        (uint32_t)challenge & UINT32_C(127);

    uint32_t key =
        (uint32_t)(challenge >> 7)
        ^ (uint32_t)(challenge >> 32);

    values[0] = phase4_walk_solution(
        target, UINT32_C(0), UINT32_C(127), key
    );

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    struct NodeData {
        uint32_t value;
        uint32_t id;
        int next;
    };

    static const struct NodeData nodes[12] = {
        {0x8A80, 0x668,  6},
        {0x806A, 0x468,  2},
        {0xC582, 0x26F,  8},
        {0x7E5C, 0x743,  0},
        {0x8385, 0xB36,  9},
        {0x5A45, 0x125, 11},
        {0x867F, 0x877,  7},
        {0xFFC7, 0x307,  5},
        {0x7B89, 0x92F, 10},
        {0x32A6, 0x00D,  1},
        {0x03D7, 0xA68, -1},
        {0x44A5, 0x568,  4}
    };

    static const int heads[8] = {
        3, 0, 6, 7, 5, 11, 4, 9
    };

    uint32_t start =
        (uint32_t)challenge & UINT32_C(7);

    uint32_t steps =
        (uint32_t)(challenge >> 3) & UINT32_C(3);

    uint32_t acc =
        (uint32_t)(challenge >> 6)
        ^ (uint32_t)(challenge >> 32)
        ^ UINT32_C(0xBB91);

    int node = heads[start];

    for (uint32_t i = 0; i <= steps; i++) {
        acc = (UINT32_C(3) * acc + nodes[node].value)
              ^ nodes[node].id;

        values[1] = nodes[node].id;

        if (i < steps) {
            node = nodes[node].next;
        }
    }

    values[0] = acc;

    return write_answer(out, cap, values, count);
}

static uint32_t phase6_apply(uint32_t index, uint32_t x) {
    switch (index) {
        case 0:
            return (UINT32_C(2) * x + UINT32_C(0xD069))
                   ^ (UINT32_C(7) * x);

        case 1:
        case 2:
            return (x >> 1) | (x << 31);

        case 3:
            return (x + UINT32_C(0x7ABC)) ^ (x >> 5);

        case 4:
        case 5:
            return ((x >> 1) ^ x) + UINT32_C(0x79D8);

        case 6:
            return ((x << 1) | (x >> 31))
                   ^ UINT32_C(0xF26B);

        case 7:
            return (x ^ UINT32_C(0x57B7))
                   + UINT32_C(4) * x;

        default:
            return 0;
    }
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t first_index =
        (uint32_t)challenge & UINT32_C(7);

    uint32_t second_index =
        (uint32_t)(challenge >> 3) & UINT32_C(7);

    uint32_t x =
        (uint32_t)(challenge >> 6)
        ^ (uint32_t)(challenge >> 32);

    uint32_t a = phase6_apply(first_index, x);

    uint32_t b = phase6_apply(
        second_index, a ^ UINT32_C(0xD16A)
    );

    values[0] = b + UINT32_C(0x95BC);

    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context,
                 char *out, size_t cap) {
    uint32_t index =
        (uint32_t)context & UINT32_C(3);

    uint32_t a =
        (uint32_t)challenge
        ^ (uint32_t)(challenge >> 32);

    uint32_t b =
        (uint32_t)context
        ^ (uint32_t)(context >> 32);

    uint32_t extra =
        (index * UINT32_C(0x223B))
        ^ (uint32_t)(context >> 16);

    uint32_t x = a ^ b;

    uint32_t mixed =
        ((x >> 1) ^ UINT32_C(0x5986))
        + (x ^ UINT32_C(0x223B));

    uint32_t second = mixed ^ extra;

    uint32_t third =
        second ^ (b + index + UINT32_C(0x5986) + (a >> 1));

    int n = snprintf(
        out, cap,
        "AAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAA"
        "OPEN"
        "%" PRIu32 " %" PRIu32 " %" PRIu32,
        index, second, third
    );

    if (n < 0 || (size_t)n >= cap) {
        return 1;
    }

    return 0;
}