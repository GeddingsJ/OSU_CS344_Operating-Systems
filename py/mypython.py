import random
import string

rand1 = ''.join([random.choice(string.lowercase) for i in xrange(10)])
rand2 = ''.join([random.choice(string.lowercase) for j in xrange(10)])
rand3 = ''.join([random.choice(string.lowercase) for k in xrange(10)])

with open('rand1.txt', 'w') as f:
	f.write("%s\n" % rand1)

with open('rand2.txt', 'w') as f:
	f.write("%s\n" % rand2)

with open('rand3.txt', 'w') as f:
	f.write("%s\n" % rand3)

print rand1
print rand2
print rand3

varNum1 = random.randint(1, 42)
varNum2 = random.randint(1, 42)

print varNum1
print varNum2

print varNum1 * varNum2
