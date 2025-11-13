def linear_search(arr, target):
    for i in range(len(arr)):
        if arr[i] == target:
            return i
    return -1

arr = [10,20,30,40,50]
target = 30

result = linear_search(arr,target)
print("index of target: ", result)
