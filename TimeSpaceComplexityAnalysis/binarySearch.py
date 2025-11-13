def binarySearch(arr, target):
    left, right = 0, len(arr) - 1
    #lenght of array is 6 but index is 5
    while left <=right:
        mid = left + (right-left) // 2

        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            left = mid + 1
        else:
            right = mid - 1
    return -1

arr = [2,4,6,8,10,12]
target = 8

result = binarySearch(arr,target)
if result != -1:
    print("element found at index ", result)
else:
    print(" Not foun")
