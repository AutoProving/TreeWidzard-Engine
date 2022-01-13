import re
m = re.match("((\d|\d\d)-)?(\d\d):(\d\d):(\d\d|\d)$","1-02:03:01")
print(m)
if m :
    print("ok")
    print(m.groups())
    print(m.group(0))
    print(m.group(1))
    print(m.group(2))
    print(m.group(3))
    print(m.group(4))
    print(m.group(5))
    print(int(m.group(5))+3)
else: 
    print("not ok")

