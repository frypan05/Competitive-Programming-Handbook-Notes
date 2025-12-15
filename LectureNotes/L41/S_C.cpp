/*
Space complexity is a measure of the total amount of memory including the 
space of input values with respect to the input size, that an algorithm 
needs to run and produce the result.



Auxiliary Space: It is the temporary or extra space used by an algorithm 
apart from the input size in order to solve a problem.

Space complexity: It is the total space used by an algorithm in order to 
solve a problem including the input size. 
It includes both auxiliary space and space taken by the input size. 

Space complexity = Auxiliary Space + Space taken by the input size

the space complexity also depends on the programming language, 
the compiler used, and the machine on which it is executed.



Let us consider a program for calculating the sum of two numbers:

three variables ‘num1’, ’num2’,’ ’sum’, considering them of data type ‘int’ 
let the space occupied by ‘int’ data type be 4 bytes, hence the total 
space consumed is 4*3 = 12 bytes.

space complexity of the above program is O(1) as 12 is a constant.



let us consider a program for calculating the sum of the numbers in an array:

We have declared variables ‘n’, ‘sum’ and array of size ‘n’, considering 
them of datatype ‘int’, let the space occupied by ‘int’ be 4 bytes, hence 
the total space consumed is 4+4+n*4 bytes. Note that the auxiliary space 
is O(1) as only the sum variable is declared apart from the input array.

Since, the space consumed is a linear function of ‘n’, the size of the 
array so the space complexity of the above program is O(n), where n is 
the size of the array.




The trade-off between Time and Space Complexity

The best algorithm to solve a particular problem is no doubt the one that 
requires less memory space and takes less time to execute. However, 
designing such an algorithm is not a trivial task, there can be more than 
one algorithm to solve a given problem one may require less memory space 
while the other may require less time to solve the problem.

So, it is quite common to observe a tradeoff between time and space 
consumed while designing an algorithm, where one needs to be sacrificed 
for the other. So, if space is a constraint, one might choose an algorithm 
that takes less space at the cost of more CPU time and vice-versa. 
Hence, we must choose an algorithm according to the requirements and the 
environment in which it needs to be executed.

*/