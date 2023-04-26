import math


# print("IADD")

num_points = 1024
a = [0] * num_points
b = [0] * num_points

for i in range(num_points):
        a[i] = (num_points // 2) - i
        b[i] = (num_points // 2) + i

# print("a", a)
# print()
# print("b", b)

# print(a[num_points-1])
# print(b[num_points-1])
print("here", a[504])

# 1535 -> 1153425408
# @512 0 -> 0000000000 -> 0x41000000 -> 0b01000001000000000000000000000000 -> iee754: 0 (technically our to float needs to understand this edge case)
# @511 1 -> 1065353216 -> 0x3f800000 -> 0b00111111100000000000000000000000 -> iee754: 1.0
# @510 2 -> 1073741824 -> 0x40000000 -> 0b01000000000000000000000000000000 -> iee754: 2.0
# @509 3 -> 1077936128 -> 0x40400000 -> 0b01000000010000000000000000000000 -> iee754: 3.0
# @508 4 -> 1082130432 -> 0x40800000 -> 0b01000000100000000000000000000000 -> iee754: 4.0
# @504 8 -> 1090519040 -> 0x41000000 -> 0b01000001000000000000000000000000 -> iee754: 8.0

####

# print("FADD")

a = [0] * num_points
b = [0] * num_points

for i in range(num_points):
    a[i] = round((num_points - i) * (1.0/num_points), 8)
    b[i] = round((num_points + i) * (1.0/num_points), 8)
    assert(a[i] + b[i] == 2)

# print("a", a)
# print()
# print("b", b)

# print(a[num_points-1])
# print(b[num_points-1])