import math
import re

print([2.5,2.2]**(1/2))
print(2 ** 3)
print(len(str(2 ** 3)))
print(math.pi)
print(math.sqrt(80))
a = "wlgdc"
print(a[-1])
print(a[len(a) - 1])
print(a[1:3])
print(a[:-1])
print(a * 2)
b = list(a)
b[1] = 't'
a.find("dc")
a.replace("w", "t")
print(a.split("g"))
print(a.upper())
print(a.isalpha())
a = a + " "
print(a.rstrip())
print("%s emm %s" % ("wc", "wc"))
print('{:2f}'.format(296.2558))
print('%.2f|%+05d' % (3.14159, -42))
print(ord("\n"))
print("\0")
a = """
can you
"""
print("\u00c4")
print(a.encode("utf8"))
print(re.match('can (,*)', a))
