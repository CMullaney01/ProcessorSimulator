int main() {
  int n = 10;
  int prev = 1;
  int curr = 1;

  while (n != 0) {
    int next = prev + curr;
    prev = curr;
    curr = next;
    n--;
  }
  return 0;
}
