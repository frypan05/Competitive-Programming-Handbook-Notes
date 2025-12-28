class Solution:
    def twoSum(self,nums, target):
        seen = {} #dictionary to store values -> index
        for i, num in enumerate(nums):
            diff = target - num
            if diff in seen:
                return [seen[diff], i]
            seen[num] = i
