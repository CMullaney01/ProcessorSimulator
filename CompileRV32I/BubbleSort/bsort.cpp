int main() {
    // array to be sorted
    int arr[5] = {5,4,3,1,2};

    // length of the array
    int l = 5;
    
    // perform bubble sort
    for (int i = 0; i < l - 1; i++) {
        for (int j = 0; j < l - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // swap elements
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

