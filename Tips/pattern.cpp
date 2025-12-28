#include<bits/stdc++.h>
using namespace std;

// 1. Solid Rectangle
void solidRectangle(int r, int c) {
    for (int i = 0; i < r; ++i)
        cout << string(c * 2, '*') << endl;
}

// 2. Hollow Rectangle
void hollowRectangle(int r, int c) {
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            if (i == 0 || i == r - 1 || j == 0 || j == c - 1)
                cout << "* ";
            else
                cout << "  ";
        }
        cout << endl;
    }
}

// 3. Half Pyramid
void halfPyramid(int n) {
    for (int i = 1; i <= n; ++i)
        cout << string(i * 2, '*') << endl;
}

// 4. Inverted Half Pyramid
void invertedHalfPyramid(int n) {
    for (int i = n; i >= 1; --i)
        cout << string(i * 2, '*') << endl;
}

// 5. Full Pyramid
void fullPyramid(int n) {
    for (int i = 1; i <= n; ++i) {
        cout << string(n - i, ' ');
        for (int j = 1; j <= 2 * i - 1; ++j)
            cout << "*";
        cout << endl;
    }
}

// 6. Inverted Full Pyramid
void invertedFullPyramid(int n) {
    for (int i = n; i >= 1; --i) {
        cout << string(n - i, ' ');
        for (int j = 1; j <= 2 * i - 1; ++j)
            cout << "*";
        cout << endl;
    }
}

// 7. Diamond Pattern
void diamond(int n) {
    for (int i = 1; i <= n; ++i)
        cout << string(n - i, ' ') << string(2 * i - 1, '*') << endl;
    for (int i = n - 1; i >= 1; --i)
        cout << string(n - i, ' ') << string(2 * i - 1, '*') << endl;
}

// 8. Number Pyramid
void numberPyramid(int n) {
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= i; ++j)
            cout << j << (j == i ? '\n' : ' ');
}

// 9. Floyd's Triangle
void floydsTriangle(int n) {
    int num = 1;
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= i; ++j)
            cout << num++ << (j == i ? '\n' : ' ');
}

// 10. 0-1 Triangle
void zeroOneTriangle(int n) {
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= i; ++j)
            cout << ((i + j) % 2 ? "0 " : "1 ");
        cout << endl;
}

// 11. Character Pyramid
void charPyramid(int n) {
    for (int i = 0; i < n; ++i) {
        char ch = 'A';
        for (int j = 0; j <= i; ++j)
            cout << ch++ << " ";
        cout << endl;
    }
}

// 12. Butterfly Pattern
void butterfly(int n) {
    for (int i = 1; i <= n; ++i)
        cout << string(i, '*') << string(2 * (n - i), ' ') << string(i, '*') << endl;
    for (int i = n; i >= 1; --i)
        cout << string(i, '*') << string(2 * (n - i), ' ') << string(i, '*') << endl;
}


int main() {
    int choice, n, r, c;

    cout << "\n🧵 DSA Pattern Machine — C++ Edition 🔧\n";
    cout << "1. Solid Rectangle\n2. Hollow Rectangle\n3. Half Pyramid\n4. Inverted Half Pyramid\n";
    cout << "5. Full Pyramid\n6. Inverted Full Pyramid\n7. Diamond\n8. Number Pyramid\n";
    cout << "9. Floyd’s Triangle\n10. 0-1 Triangle\n11. Character Pyramid\n12. Butterfly Pattern\n";
    cout << "Choose pattern (1-12): ";
    cin >> choice;

    switch (choice) {
        case 1: cout << "Rows & Cols: "; cin >> r >> c; solidRectangle(r, c); break;
        case 2: cout << "Rows & Cols: "; cin >> r >> c; hollowRectangle(r, c); break;
        case 3: cout << "Height: "; cin >> n; halfPyramid(n); break;
        case 4: cout << "Height: "; cin >> n; invertedHalfPyramid(n); break;
        case 5: cout << "Height: "; cin >> n; fullPyramid(n); break;
        case 6: cout << "Height: "; cin >> n; invertedFullPyramid(n); break;
        case 7: cout << "Height: "; cin >> n; diamond(n); break;
        case 8: cout << "Height: "; cin >> n; numberPyramid(n); break;
        case 9: cout << "Height: "; cin >> n; floydsTriangle(n); break;
        case 10: cout << "Height: "; cin >> n; zeroOneTriangle(n); break;
        case 11: cout << "Height: "; cin >> n; charPyramid(n); break;
        case 12: cout << "Height: "; cin >> n; butterfly(n); break;
        default: cout << "Invalid option, bro. Try again.\n";
    }
    return 0;
}

