def max_in_array(arr):
    max = arr[0]

    for i in range (0, len(arr) -1):
        if arr[i] > max:
            max = arr[i]
    return max
arr = [2,4,1,5,16,14,10]

result = max_in_array(arr)

print("max element in array: ", result)
