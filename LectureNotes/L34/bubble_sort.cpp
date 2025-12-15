#include <bits/stdc++.h>
using namespace std;

void BubbleSort(int *arr, int n)
{

    // base case - already sorted
    if (n < 2)
    {
        return;
    }

    // 1 case solve karlia  - largest element ko end mein rakh dega

    for (int i = 0; i < n - 1; i++)
    {
        if (arr[i] > arr[i + 1])
        {
            swap(arr[i], arr[i + 1]);
        }
    }

    BubbleSort(arr, n - 1);
}

int main()
{

    int arr[5] = {2, 5, 1, 6, 9};

    BubbleSort(arr, 5);

    for (int i = 0; i < 5; i++)
    {
        cout << arr[i] << endl;
    }

    return 0;
}