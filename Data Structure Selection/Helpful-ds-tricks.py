# if asked to know whther the list contains duplicates, we can use the following code snippet:
#
"""
def has_duplicates(lst):
    if (len(set(nums))) == len(nums):
        return False
    else:
        return True
"""

# for the question:
# Input: nums = [4,3,2,7,8,2,3,1]
# Output: [5,6]
#
"""
set_nums = set(nums)
ret = []

for i in range (1, len(nums)+1):
    if i not in set_nums:
        ret.append(i)
return ret
"""
