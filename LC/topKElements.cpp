class Solution{
    public:
        vector<int> topKElements(vector<int>& nums, int k) {
            priority_queue<int, vector<int>, greater<int>> minHeap;

            for (int num : nums) {
                minHeap.push(num);
                if (minHeap.size() > k) {
                    minHeap.pop();
                }
            }

            vector<int> result;
            while (!minHeap.empty()) {
                result.push_back(minHeap.top());
                minHeap.pop();
            }

            return result;
        }
}
