
arr = [3, 2, 4, 1, 5]
l = 5;

# bubble sort
for i in range(l):
    for j in range(l - 1):
        if arr[j] > arr[j + 1]:
            arr[j], arr[j + 1] = arr[j + 1], arr[j]


if (arr[0] < arr[1] and arr[1] < arr[2] and arr[2] < arr[3] and arr[3] < arr[4]):
    print("Sorted, ", arr);