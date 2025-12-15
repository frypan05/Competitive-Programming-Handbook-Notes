//How to Compare Algorithms?

//To compare algorithms, let us define a few objective measures
//Execution Times? Not a good measure as execution time are specific to a 
//particular computer
//Number of statements executed? Not a good measure as the number of statements varies with 
//programming languages as well as with the style of individual programmer
//Ideal Solution? Let us assume that we express the running time of a given 
//algorithm as a function of input size n(i.e., f(n)) and compare these 
//different functions corresponding to running times. 
//This kind of comparison is independent of machine time, programming style, etc. 
//We measure the total number of basic operations (additions, subtractions, 
//increments, multiplications, divisions,modulo etc.) performed as a function of input size. 



//The rate at which running time increases as a function of input is called rate of growth. 
//
//Let us assume that you go to a shop to buy a car and a bicycle. If your friend sees you 
//over there and asks you what you are buying, then in general you say buying a car. This 
//is because the cost of the car is high compared to the cost of the bicycle 
//(approximately the cost of the bicycle to the cost of the car).
//
//Total cost = cost of car + cost of bicycle
//Total cost ~ cost of car (approximation)
//Similarly, 
//For a given function, ignore the low order terms that are relatively insignificant.
//n4 + 2n3 + 100n + 500 ~ n4 (for large value of input size, n)


/*
Big-O Notation

This notation gives the tight upper bound of the given function.
That means at larger values of N the upper bound of a f(n) is g(n). 
represented as f(n)=O(g(n))
if f(n) =  n^4 + 2n^3 + 100n + 500 is the given algorithm then n4 is g(n)


Omega-Ω Notation

Similar to the O discussion, this notation gives the tighter lower bound of the given algorithm
at larger values of n, the tighter lower bound of f(n) is g(n).
we represent it as f(n) = Ω(g(n))
For example, if f(n) = 100n^2+10n+50, g(n) is Ω(n^2).


Theta-Θ Notation

This notation decides whether the upper and the lower bound of the given
function(algorithm) are the same. The average running time of an algorithm 
is always between the lower bound and the upper bound




Loops: The running time of a loop is, at most, the running time of the 
statements inside the loop (including tests) multiplied by the number 
of iterations .
//   executes n times
for i from 1 to n
        m = m + 2;        //   constant time c
        i++;
Total time = (a constant c) x n = cn = O(n) 



Nested loops: Analyze from the inside out. Total running time is the 
product of the sizes of all the loops
//   outer loop executed n times
for i from 1 to n {
       //   inner loop executed n time
       i++;
       for j from 1 to n {
               k = k+1;     //   constant time
               j++
       }
}
Total time = c x n x n = cn^2 = O(n^2)


Consecutive Statements: Add the time complexities of each statement.

x = x + 1;                    //  constant time
//   executed n times
for i from 1 to n {
        m = m + 2;        //   constant time c
        i++;
}
//   outer loop executed n time
for i from 1 to n {
       //   inner loop executed n time
       for j from 1 to n {
               k = k + 1;    //   constant time
               j++;
        }
}
Total time = c0 + c1n + c2n^2 = O(n^2)



If-then-else statements:  Worst case running time: the test, plus either 
the then part or the else part (whichever is the largest)

//   condition check: constant time
if(length() == 0)
       return false;
else { 
       // else part: (constant + constant) * n
       for n = 0 to n< length {
             //   another if: constant + constant (no else part)
             if( !list[n].equals(otherList.list[n]) )
                     return false;
       }
}

Total time = c0 + (c1 + c2) * n = O(n)


Logarithmic Complexity: Algorithm is O(logn) if it takes a constant time 
to cut the problem size by a fraction(usually by 1/2).

for i from 1 to n
      i = i * 2

At the kth step 2k = n, and at (k+1)th step we come out of the loop.

Total time = O(logn)


*/




